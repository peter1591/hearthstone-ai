#pragma once

#include <istream>
#include <ostream>
#include <random>

#include "UI/AIController.h"
#include "UI/ActionApplyHelper.h"

namespace ui
{
	class InteractiveShell
	{
	public:
		using StartBoardGetter = std::function<state::State(int)>;

		InteractiveShell(AIController * controller = nullptr, StartBoardGetter start_board_getter = StartBoardGetter()) :
			controller_(controller), start_board_getter_(start_board_getter), node_(nullptr)
		{
			try {
				state_value_func_.reset(new mcts::policy::simulation::NeuralNetworkStateValueFunction());
			}
			catch (std::exception ex) {
				state_value_func_.reset(nullptr);
			}
		}

		InteractiveShell(InteractiveShell const&) = delete;
		InteractiveShell & operator=(InteractiveShell const&) = delete;

		void SetController(AIController * controller) {
			controller_ = controller;
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
				node_ = (mcts::builder::TreeBuilder::TreeNode *)(v);
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
			const mcts::selection::TreeNode* node,
			ui::ActionApplyHelper const& action_cb_info_getter,
			int indent,
			bool only_show_best_choice = true)
		{
			std::string indent_padding;
			for (int i = 0; i < indent; ++i) indent_padding.append("   ");

			int64_t total_chosen_time = 0;
			int best_choice = -1;
			int64_t best_choice_chosen_times = 0;
			node->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				auto const* edge_addon = node->GetEdgeAddon(choice);
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
			node->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				if (only_show_best_choice) {
					if (choice != best_choice) return true;
				}

				s << indent_padding << "Choice " << choice
					<< ": " << GetChoiceString(main_node, node, choice, action_cb_info_getter)
					<< " " << GetChoiceSuggestionRate(node, choice, total_chosen_time)
					<< std::endl;
				ui::ActionApplyHelper new_cb_getter = action_cb_info_getter;
				new_cb_getter.AppendChoice(choice);
				return ShowBestSubNodeInfo(s, main_node, node, choice, total_chosen_time, child, new_cb_getter, indent+1, only_show_best_choice);
			});
		}

		std::string GetTargetString(state::CardRef card_ref) {

			state::State board = start_board_getter_(0);
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
				ss << "Your " << (idx+1) << "th minion";
				return ss.str();
			}

			idx = FindInMinions(board.GetBoard().GetSecond().minions_.GetAll());
			if (idx >= 0) {
				std::stringstream ss;
				ss << "Opponent's " << (idx+1) << "th minion";
				return ss.str();
			}

			return "[UNKNOWN TARGET]";
		}

		std::string GetTargetStringFromEncodedIndex(int idx) {
			// encoded index is defined in FlowControl::ValidActionGetter
			// encoded index:
			//   0 ~ 6: minion index from left to right
			//   7: hero
			if (idx == 7) return "Your Hero";
			assert(idx >= 0);
			assert(idx <= 6);
			std::stringstream ss;
			ss << "Your " << (idx + 1) << "th Minion";
			return ss.str();
		}

		std::string GetChoiceString(
			const mcts::selection::TreeNode* main_node,
			const mcts::selection::TreeNode* node, int choice,
			ui::ActionApplyHelper const& action_cb_info_getter)
		{
			if (node->GetActionType() == mcts::ActionType::kMainAction) {
				auto op = node->GetAddon().action_analyzer.GetMainOpType(choice);
				return GetOpType(op);
			}
			
			if (node->GetActionType() == mcts::ActionType::kChooseHandCard) {
				auto info = std::get<ui::ActionApplyHelper::ChooseHandCardInfo>(
					action_cb_info_getter.ApplyChoices([&]() {
					return start_board_getter_(0); // any seed number is okay
				}));
				size_t idx = info.cards[choice];

				state::State start_board = start_board_getter_(0);
				state::CardRef card_ref = start_board.GetBoard().GetFirst().hand_.Get(idx);
				auto card_id = start_board.GetCard(card_ref).GetCardId();

				return Cards::Database::GetInstance().Get((int)card_id).name;
			}

			if (node->GetActionType() == mcts::ActionType::kChooseMinionPutLocation) {
				auto info = std::get<ui::ActionApplyHelper::MinionPutLocationInfo>(
					action_cb_info_getter.ApplyChoices([&]() {
					return start_board_getter_(0); // any seed number is okay
				}));
				std::stringstream ss;
				ss << "Put minion before index " << choice
					<< " (total " << info.minions << ")";
				return ss.str();
			}

			if (node->GetActionType() == mcts::ActionType::kChooseAttacker) {
				auto info = std::get<ui::ActionApplyHelper::ChooseAttackerInfo>(
					action_cb_info_getter.ApplyChoices([&]() {
					return start_board_getter_(0); // any seed number is okay
				}));
				std::stringstream ss;
				ss << "Attacker: " << GetTargetStringFromEncodedIndex(info.targets[choice]);
				return ss.str();
			}

			if (node->GetActionType() == mcts::ActionType::kChooseDefender) {
				auto info = std::get<ui::ActionApplyHelper::ChooseDefenderInfo>(
					action_cb_info_getter.ApplyChoices([&]() {
					return start_board_getter_(0); // any seed number is okay
				}));
				std::stringstream ss;
				ss << "Defender: " << GetTargetString(info.targets[choice]);
				return ss.str();
			}

			if (node->GetActionType() == mcts::ActionType::kChooseTarget) {
				auto info = std::get<ui::ActionApplyHelper::GetSpecifiedTargetInfo>(
					action_cb_info_getter.ApplyChoices([&]() {
					return start_board_getter_(0); // any seed number is okay
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
			auto const* edge_addon = node->GetEdgeAddon(choice);
			auto chosen_times = edge_addon->GetChosenTimes();
			double chosen_percent = 100.0 * chosen_times / total_chosen_times;

			std::stringstream ss;
			ss << chosen_percent << "% (" << chosen_times << "/" << total_chosen_times << ")";
			return ss.str();
		}

		bool ShowBestSubNodeInfo(
			std::ostream & s,
			const mcts::selection::TreeNode* main_node,
			const mcts::selection::TreeNode* node,
			int choice,
			uint64_t total_chosen_times,
 			mcts::selection::ChildType const& child,
			ui::ActionApplyHelper const& action_cb_info_getter,
			int indent,
			bool only_show_best_choice = true)
		{
			std::string indent_padding;
			for (int i = 0; i < indent; ++i) indent_padding.append("   ");

			auto const* edge_addon = node->GetEdgeAddon(choice);
			if (!edge_addon) {
				s << "[ERROR] Children of main action should with edge addon" << std::endl;
				return false;
			}

			if (!only_show_best_choice) {
				double credit_percentage = (double)edge_addon->GetCredit() / edge_addon->GetTotal() * 100;
				s << indent_padding
					<< "Estimated win rate: " << edge_addon->GetCredit() << " / " << edge_addon->GetTotal() << " (" << credit_percentage << "%)"
					<< std::endl;
			}

			if (!child.IsRedirectNode()) {
				ShowBestNodeInfo(s, main_node, child.GetNode(), action_cb_info_getter, indent, only_show_best_choice);
			}
			else {
				if (!only_show_best_choice) {
					state::State game_state = start_board_getter_(0); // any seed number is okay
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

			if (verbose) {
				state::State game_state = start_board_getter_(0);
				double v = GetStateValue(game_state);
				s << "State-value: " << v << std::endl;
			}

			auto node = controller_->GetRootNode(state::kPlayerFirst);
			if (!node) {
				s << "[ERROR] no root node exists." << std::endl;
				return;
			}

			if (node->GetActionType() != mcts::ActionType::kMainAction) {
				s << "[ERROR] root node should be with type 'kMainAction'" << std::endl;
				return;
			}

			ui::ActionApplyHelper action_cb_info_getter;
			ShowBestNodeInfo(s, node, node, action_cb_info_getter, 0, !verbose);
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
			if (!node_) {
				s << "Should set node first." << std::endl;
				return;
			}

			s << "Action type: " << GetActionType(node_->GetActionType()) << std::endl;

			if (node_->GetActionType() == mcts::ActionType::kMainAction) {
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

				if (node_->GetActionType() == mcts::ActionType::kMainAction) {
					auto op = node_->GetAddon().action_analyzer.GetMainOpType(choice);
					s << "    Main Action Op: " << GetOpType(op) << std::endl;
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
			node_->GetAddon().board_node_map.ForEach([&](mcts::board::BoardView board_view, mcts::builder::TreeBuilder::TreeNode* node) {
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
		}

		std::string GetActionType(mcts::ActionType type) {
			using mcts::ActionType;
			switch (type.GetType()) {
			case ActionType::kInvalid: return "kInvalid";
			case ActionType::kMainAction: return "kMainAction";
			case ActionType::kRandom: return "kRandom";
			case ActionType::kChooseHandCard: return "kChooseHandCard";
			case ActionType::kChooseAttacker: return "kChooseAttacker";
			case ActionType::kChooseDefender: return "kChooseDefender";
			case ActionType::kChooseMinionPutLocation: return "kChooseMinionPutLocation";
			case ActionType::kChooseTarget: return "kChooseTarget";
			case ActionType::kChooseOne: return "kChooseOne";
			default: return "Unknown!!!";
			}
		}

		std::string GetOpType(mcts::board::BoardActionAnalyzer::OpType op) {
			using mcts::board::BoardActionAnalyzer;
			switch (op) {
			case BoardActionAnalyzer::kInvalid: return "kInvalid";
			case BoardActionAnalyzer::kPlayCard: return "kPlayCard";
			case BoardActionAnalyzer::kAttack: return "kAttack";
			case BoardActionAnalyzer::kHeroPower: return "kHeroPower";
			case BoardActionAnalyzer::kEndTurn: return "kEndTurn";
			default: return "Unknown!!!";
			}
		}

		void PrintBoardView(mcts::board::BoardView const& view, std::string const& line_prefix, std::ostream & s) {
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
		ui::AIController * controller_;
		StartBoardGetter start_board_getter_;
		mcts::builder::TreeBuilder::TreeNode const* node_;
		std::unique_ptr<mcts::policy::simulation::NeuralNetworkStateValueFunction> state_value_func_;
	};
}