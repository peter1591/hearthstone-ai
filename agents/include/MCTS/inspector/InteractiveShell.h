#pragma once

#include <istream>
#include <ostream>
#include <random>

#include "agents/MCTSRunner.h"
#include "engine/ActionApplyHelper.h"

namespace mcts
{
	namespace inspector
	{
		class InteractiveShell
		{
		public:
			using StartBoardGetter = std::function<state::State(std::mt19937 &)>;

			InteractiveShell(
				agents::MCTSRunner * controller = nullptr,
				StartBoardGetter start_board_getter = StartBoardGetter()) :
				controller_(controller), start_board_getter_(start_board_getter), node_(nullptr)
			{
			}

			InteractiveShell(InteractiveShell const&) = delete;
			InteractiveShell & operator=(InteractiveShell const&) = delete;

			void SetController(agents::MCTSRunner * controller) {
				controller_ = controller;
			}

			void SetConfig(agents::MCTSAgentConfig const& config) {
				try {
					state_value_func_.reset(new mcts::policy::simulation::NeuralNetworkStateValueFunction(config.mcts));
				}
				catch (std::exception ex) {
					state_value_func_.reset(nullptr);
				}
			}

			void SetStartBoardGetter(StartBoardGetter start_board_getter) {
				start_board_getter_ = start_board_getter;
			}

			void DoCommand(std::istream & is, std::ostream & s)
			{
				std::string cmd;
				is >> cmd;
				if (cmd == "h" || cmd == "help") {
					s << "Commands: " << std::endl
						<< "h or help: show this message" << std::endl
						<< "b or best: show the best action to do (add -v for verbose)" << std::endl
						<< "root (1 or 2): set node to root node of player 1 or 2" << std::endl
						<< "info: show info for selected node" << std::endl
						<< "node (addr): set node to specified address." << std::endl;
				}
				else if (cmd == "b" || cmd == "best") {
					DoBest(is, s);
				}
				else if (cmd == "root") {
					DoRoot(is, s);
				}
				else if (cmd == "info") {
					DoInfo(s);
				}
				else if (cmd == "node") {
					uint64_t v = 0;
					is >> std::hex >> v >> std::dec;
					node_ = (mcts::selection::TreeNode *)(v);
					s << "Node set to: " << node_ << std::endl;
				}
				else {
					s << "Unknown command. Type 'h' or 'help' for usage help." << std::endl;
				}
			}

		private:
			double GetStateValue(state::State const& s) {
				if (!state_value_func_) return std::numeric_limits<double>::quiet_NaN();
				return state_value_func_->GetStateValue(s);
			}

			void ShowBestNodeInfo(
				std::ostream & s,
				const mcts::selection::TreeNode* main_node,
				engine::ValidActionAnalyzer const& action_analyzer,
				const mcts::selection::TreeNode* node,
				engine::ActionApplyHelper const& action_cb_info_getter,
				int indent,
				bool only_show_best_choice = true)
			{
				std::string indent_padding;
				for (int i = 0; i < indent; ++i) indent_padding.append("   ");

				int64_t total_chosen_time = 0;
				int best_choice = -1;
				int64_t best_choice_chosen_times = 0;
				node->children_.ForEach([&](int choice, mcts::selection::EdgeAddon const* edge_addon, mcts::selection::TreeNode* child) {
					if (edge_addon) {
						auto chosen_times = edge_addon->GetChosenTimes();
						total_chosen_time += chosen_times;

						if (chosen_times > best_choice_chosen_times) {
							best_choice_chosen_times = chosen_times;
							best_choice = choice;
						}
					}
					return true;
				});
				node->children_.ForEach([&](int choice, mcts::selection::EdgeAddon const* edge_addon, mcts::selection::TreeNode* child) {
					if (only_show_best_choice) {
						if (choice != best_choice) return true;
					}

					s << indent_padding << "Choice " << choice
						<< ": " << GetChoiceString(main_node, action_analyzer, node, choice, action_cb_info_getter)
						<< " " << GetChoiceSuggestionRate(node, choice, total_chosen_time)
						<< std::endl;
					engine::ActionApplyHelper new_cb_getter = action_cb_info_getter;
					new_cb_getter.AppendChoice(choice);
					return ShowBestSubNodeInfo(s, main_node, action_analyzer, node, choice, total_chosen_time,
						edge_addon, child, new_cb_getter, indent + 1, only_show_best_choice);
				});
			}

			std::string GetTargetString(state::CardRef card_ref) {
				std::mt19937 rand; // not random seeded; just to get one of the possible boards
				state::State board = start_board_getter_(rand);
				if (board.GetBoard().GetFirst().GetHeroRef() == card_ref) {
					return "Your Hero";
				}
				if (board.GetBoard().GetSecond().GetHeroRef() == card_ref) {
					return "Opponent Hero";
				}

				auto FindInMinions = [&](std::vector<state::CardRef> const& minions) -> int {
					for (size_t i = 0; i < minions.size(); ++i) {
						if (minions[i] == card_ref) return (int)i;
					}
					return -1;
				};

				int idx = FindInMinions(board.GetBoard().GetFirst().minions_.GetAll());
				if (idx >= 0) {
					std::stringstream ss;
					ss << "Your " << (idx + 1) << "th minion";
					return ss.str();
				}

				idx = FindInMinions(board.GetBoard().GetSecond().minions_.GetAll());
				if (idx >= 0) {
					std::stringstream ss;
					ss << "Opponent's " << (idx + 1) << "th minion";
					return ss.str();
				}

				return "[UNKNOWN TARGET]";
			}

			std::string GetTargetStringFromEncodedIndex(int idx) {
				std::string side;
				if (idx >= 8) {
					side = "Opponent";
					idx -= 8;
				}
				else {
					side = "Your";
				}
				// encoded index is defined in FlowControl::ValidActionGetter
				// encoded index:
				//   0 ~ 6: minion index from left to right
				//   7: hero
				if (idx == 7) return side + " Hero";
				assert(idx >= 0);
				assert(idx <= 6);
				std::stringstream ss;
				ss << side << " " << (idx + 1) << "th Minion";
				return ss.str();
			}

			std::string GetChoiceString(
				const mcts::selection::TreeNode* main_node,
				engine::ValidActionAnalyzer const& action_analyzer,
				const mcts::selection::TreeNode* node, int choice,
				engine::ActionApplyHelper const& action_cb_info_getter)
			{
				auto action_type = node->addon_.consistency_checker.GetActionType();
				if (action_type == engine::ActionType::kMainAction) {
					auto op = action_analyzer.GetMainOpType(choice);
					return engine::GetMainOpString(op);
				}

				if (action_type == engine::ActionType::kChooseHandCard) {
					auto const& playable_cards = action_analyzer.GetPlayableCards();
					size_t idx = playable_cards[choice];

					std::mt19937 rand; // not random seeded; just to get one of the possible boards
					state::State start_board = start_board_getter_(rand);
					state::CardRef card_ref = start_board.GetBoard().GetFirst().hand_.Get(idx);
					auto card_id = start_board.GetCard(card_ref).GetCardId();

					return Cards::Database::GetInstance().Get((int)card_id).name;
				}

				if (action_type == engine::ActionType::kChooseMinionPutLocation) {
					auto v = action_cb_info_getter.ApplyChoices([&]() {
						std::mt19937 rand; // not random seeded; just to get one of the possible boards
						return start_board_getter_(rand);
					});
					auto info = std::get<engine::ActionApplyHelper::MinionPutLocationInfo>(v);
					std::stringstream ss;
					ss << "Put minion before index " << choice
						<< " (total " << info.minions << ")";
					return ss.str();
				}

				if (action_type == engine::ActionType::kChooseAttacker) {
					std::stringstream ss;
					ss << "Attacker: " << choice;
					return ss.str();
				}

				if (action_type == engine::ActionType::kChooseDefender) {
					auto info = std::get<engine::ActionApplyHelper::ChooseDefenderInfo>(
						action_cb_info_getter.ApplyChoices([&]() {
						std::mt19937 rand; // not random seeded; just to get one of the possible boards
						return start_board_getter_(rand);
					}));
					std::stringstream ss;
					ss << "Defender: " << GetTargetStringFromEncodedIndex(info.targets[choice]);
					return ss.str();
				}

				if (action_type == engine::ActionType::kChooseTarget) {
					auto info = std::get<engine::ActionApplyHelper::GetSpecifiedTargetInfo>(
						action_cb_info_getter.ApplyChoices([&]() {
						std::mt19937 rand; // not random seeded; just to get one of the possible boards
						return start_board_getter_(rand);
					}));
					std::stringstream ss;
					ss << "Target at [" << GetTargetString(info.targets[choice]) << "]";
					return ss.str();
				}

				return "(unknown)";
			}

			std::string GetChoiceSuggestionRate(
				const mcts::selection::TreeNode* node,
				int choice,
				uint64_t total_chosen_times)
			{
				auto const* edge_addon = node->children_.GetEdgeAddon(choice);
				auto chosen_times = edge_addon->GetChosenTimes();
				double chosen_percent = 100.0 * chosen_times / total_chosen_times;

				std::stringstream ss;
				ss << chosen_percent << "% (" << chosen_times << "/" << total_chosen_times << ")";
				return ss.str();
			}

			bool ShowBestSubNodeInfo(
				std::ostream & s,
				const mcts::selection::TreeNode* main_node,
				engine::ValidActionAnalyzer const& action_analyzer,
				const mcts::selection::TreeNode* node,
				int choice,
				uint64_t total_chosen_times,
				mcts::selection::EdgeAddon const* edge_addon,
				mcts::selection::TreeNode * child,
				engine::ActionApplyHelper const& action_cb_info_getter,
				int indent,
				bool only_show_best_choice = true)
			{
				std::string indent_padding;
				for (int i = 0; i < indent; ++i) indent_padding.append("   ");

				if (!edge_addon) {
					s << "[ERROR] Children of main action should with edge addon" << std::endl;
					return false;
				}

				if (!only_show_best_choice) {
					s << indent_padding
						<< "Estimated win rate: " << edge_addon->GetAverageCredit() * 100.0 << "%"
						<< std::endl;
				}

				if (child) {
					ShowBestNodeInfo(s, main_node, action_analyzer, child, action_cb_info_getter, indent, only_show_best_choice);
				}
				else {
					if (!only_show_best_choice) {
						std::mt19937 rand; // not random seeded; just to get one of the possible boards
						state::State game_state = start_board_getter_(rand);
						auto dummy_cb_info = action_cb_info_getter.ApplyChoices(game_state);
						double v = GetStateValue(game_state);
						s << indent_padding << "State-value: " << v << std::endl;
					}
				}
				return true;
			}

			void DoBest(std::istream& is, std::ostream & s)
			{
				std::string str_verbose;
				if (is) is >> str_verbose;

				bool verbose = false;
				if (str_verbose == "-v") {
					verbose = true;
				}
				else if (str_verbose.empty()) {
					verbose = false;
				}
				else {
					s << "Unknown option: " << str_verbose;
					return;
				}

				s << "Best action: " << std::endl;

				std::mt19937 rand; // not random seeded; just to get one of the possible boards
				state::State game_state = start_board_getter_(rand);
				engine::ValidActionAnalyzer action_analyzer;
				action_analyzer.Analyze(game_state);
				if (verbose) {
					double v = GetStateValue(game_state);
					s << "State-value: " << v << std::endl;
				}

				auto node = controller_->GetRootNode(state::kPlayerFirst);
				if (!node) {
					s << "[ERROR] no root node exists." << std::endl;
					return;
				}

				if (!node->addon_.consistency_checker.CheckActionType(engine::ActionType::kMainAction)) {
					s << "[ERROR] root node should be with type 'kMainAction'" << std::endl;
					return;
				}

				engine::ActionApplyHelper action_cb_info_getter;
				ShowBestNodeInfo(s, node, action_analyzer, node, action_cb_info_getter, 0, !verbose);
			}

			void DoRoot(std::istream & is, std::ostream & s)
			{
				int v = 0;
				is >> v;

				state::PlayerIdentifier side;
				if (v == 1) side = state::PlayerIdentifier::First();
				else if (v == 2) side = state::PlayerIdentifier::Second();
				else {
					s << "invalid input" << std::endl;
					return;
				}
				node_ = controller_->GetRootNode(side);
				s << "Current node set to: " << node_ << std::endl;
			}

			void DoInfo(std::ostream & s)
			{
				s << "Action analyzer is no longer recorded in treenode. We cannot generate useful action info for an arbitrarily node anymore." << std::endl;
				/*
				if (!node_) {
					s << "Should set node first." << std::endl;
					return;
				}

				s << "Action type: " << GetActionType(node_->GetActionType()) << std::endl;

				if (node_->GetActionType() == engine::ActionType::kMainAction) {
					s << "Playable hand cards:";
					auto * board_view = node_->GetAddon().consistency_checker.GetBoard();
					node_->GetAddon().action_analyzer.ForEachPlayableCard([&](size_t idx) {
						s << " [" << idx << "] ";
						if (board_view) {
							s << board_view->GetSelfHand()[idx].card_id;
						}
						return true;
					});
					s << std::endl;
				}

				s << "Children:" << std::endl;
				node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
					auto const* edge_addon = node_->GetEdgeAddon(choice);

					if (child.IsRedirectNode()) {
						s << "  " << choice << ": [REDIRECT]" << std::endl;
					}
					else {
						s << "  " << choice << ": [NORMAL] "
							<< child.GetNode() << std::endl;
					}

					if (node_->GetActionType() == engine::ActionType::kMainAction) {
						auto op = node_->GetAddon().action_analyzer.GetMainOpType(choice);
						s << "    Main Action Op: " << FlowControl::GetMainOpString(op) << std::endl;
					}

					if (edge_addon) {
						s << "    Chosen time: " << edge_addon->GetChosenTimes() << std::endl;

						double credit_percentage = (double)edge_addon->GetCredit() / edge_addon->GetTotal() * 100;
						s << "    Credit: " << edge_addon->GetCredit() << " / " << edge_addon->GetTotal()
							<< " (" << credit_percentage << "%)"
							<< std::endl;
					}
					return true;
				});
				s << "BoardNodeMap:" << std::endl;
				node_->GetAddon().board_node_map.ForEach([&](judge::view::BoardView board_view, mcts::builder::TreeBuilder::TreeNode* node) {
					uint64_t total_chosen_time = 0;
					node->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
						total_chosen_time += child.GetEdgeAddon().GetChosenTimes();
						return true;
					});
					s << "  " << node << " Chosen time: " << total_chosen_time
						<< std::endl;
					PrintBoardView(board_view, "    ", s);
					return true;
				});
				*/
			}

			std::string GetActionType(engine::ActionType type) {
				using engine::ActionType;
				switch (type.GetType()) {
				case engine::ActionType::kInvalid: return "kInvalid";
				case engine::ActionType::kMainAction: return "kMainAction";
				case engine::ActionType::kRandom: return "kRandom";
				case engine::ActionType::kChooseHandCard: return "kChooseHandCard";
				case engine::ActionType::kChooseAttacker: return "kChooseAttacker";
				case engine::ActionType::kChooseDefender: return "kChooseDefender";
				case engine::ActionType::kChooseMinionPutLocation: return "kChooseMinionPutLocation";
				case engine::ActionType::kChooseTarget: return "kChooseTarget";
				case engine::ActionType::kChooseOne: return "kChooseOne";
				default: return "Unknown!!!";
				}
			}

			void PrintBoardView(engine::view::ReducedBoardView const& view, std::string const& line_prefix, std::ostream & s) {
				s << line_prefix << "Side: " << GetSideStr(view.GetSide())
					<< std::endl;

				s << line_prefix << "Self resource: "
					<< view.GetSelfCrystal().current << " / " << view.GetSelfCrystal().total
					<< std::endl;

				s << line_prefix << "Self Hero: "
					<< view.GetSelfHero().hp << " / " << view.GetSelfHero().max_hp
					<< " armor: " << view.GetSelfHero().armor
					<< std::endl;

				s << line_prefix << "Self hand cards: ";
				for (auto const& card : view.GetSelfHand()) {
					s << card.card_id << " ";
				}
				s << std::endl;

				s << line_prefix << "Self Minions: ";
				for (auto const& minion : view.GetSelfMinions()) {
					s << minion.attack << "/" << minion.hp << "(" << minion.max_hp << ") ";
				}
				s << std::endl;

				s << line_prefix << "Opponent resource: "
					<< view.GetOpponentCrystal().current << " / " << view.GetOpponentCrystal().total
					<< std::endl;

				s << line_prefix << "Opponent Hero: "
					<< view.GetOpponentHero().hp << " / " << view.GetOpponentHero().max_hp
					<< " armor: " << view.GetOpponentHero().armor
					<< std::endl;

				s << line_prefix << "Opponent Minions: ";
				for (auto const& minion : view.GetOpponentMinions()) {
					s << minion.hp << "/" << minion.max_hp << " ";
				}
				s << std::endl;
			}

			std::string GetSideStr(state::PlayerSide side) {
				if (side == state::kPlayerFirst) return "kPlayerFirst";
				if (side == state::kPlayerSecond) return "kPlayerSecond";
				if (side == state::kPlayerInvalid) return "kPlayerInvalid";
				return "Unknown!!!";
			}

		private:
			agents::MCTSRunner * controller_;
			StartBoardGetter start_board_getter_;
			mcts::selection::TreeNode const* node_;
			std::unique_ptr<mcts::policy::simulation::NeuralNetworkStateValueFunction> state_value_func_;
		};
	}
}