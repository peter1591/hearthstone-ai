#include "FlowControl/FlowController-impl.h"

#include <chrono>
#include <iostream>
#include <sstream>

#include "Cards/PreIndexedCards.h"
#include "MCTS/TestStateBuilder.h"
#include "UI/AIController.h"
#include "UI/CompetitionGuide.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
}

class Handler
{
public:
	Handler() : controller_(), node_(nullptr) {}

	Handler(Handler const&) = delete;
	Handler & operator=(Handler const&) = delete;

	void Start()
	{
		auto start_board_getter = [](int seed) -> state::State {
			return TestStateBuilder().GetState(seed);
		};

		int threads = 1;
		while (std::cin) {
			std::string cmd;
			std::cout << "Command: ";
			std::cin >> cmd;

			if (cmd == "h" || cmd == "help") {
				std::cout << "Commands: " << std::endl
					<< "h or help: show this message" << std::endl
					<< "threads: set number of threads" << std::endl
					<< "s or start (secs): to run for a specified seconds" << std::endl
					<< "root (1 or 2): set node to root node of player 1 or 2" << std::endl
					<< "info: show info for selected node" << std::endl
					<< "node (addr): set node to specified address." << std::endl
					<< "q or quit: quit" << std::endl;
			}
			else if (cmd == "threads") {
				std::cin >> threads;
				std::cout << "Set thread count to " << threads << std::endl;
			}
			else if (cmd == "s" || cmd == "start") {
				int secs = 0;
				std::cin >> secs;

				std::cout << "Running for " << secs << " seconds with " << threads << " threads." << std::endl;

				auto start = std::chrono::steady_clock::now();
				std::chrono::steady_clock::time_point run_until =
					std::chrono::steady_clock::now() +
					std::chrono::seconds(secs);

				long long last_show_rest_sec = -1;
				auto continue_checker = [&]() {
					auto now = std::chrono::steady_clock::now();
					if (now > run_until) return false;

					auto rest_sec = std::chrono::duration_cast<std::chrono::seconds>(run_until - now).count();
					if (rest_sec != last_show_rest_sec) {
						std::cout << "Rest seconds: " << rest_sec << std::endl;
						last_show_rest_sec = rest_sec;
					}
					return true;
				};

				auto start_i = controller_.GetStatistic().GetSuccededIterates();
				controller_.Run(continue_checker, threads, start_board_getter);
				auto end_i = controller_.GetStatistic().GetSuccededIterates();

				std::cout << std::endl;
				std::cout << "Done iterations: " << (end_i - start_i) << std::endl;
				std::cout << "====== Statistics =====" << std::endl;
				controller_.GetStatistic().PrintMessage();

				auto now = std::chrono::steady_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
				auto speed = (double)(end_i - start_i) / ms * 1000;
				std::cout << "Iterations per second: " << speed << std::endl;
				std::cout << std::endl;
			}
			else if (cmd == "root") {
				DoRoot();
			}
			else if (cmd == "info") {
				DoInfo();
			}
			else if (cmd == "node") {
				uint64_t v = 0;
				std::cin >> std::hex >> v >> std::dec;
				node_ = (mcts::builder::TreeBuilder::TreeNode *)(v);
				std::cout << "Node set to: " << node_ << std::endl;
			}
			else if (cmd == "q" || cmd == "quit") {
				std::cout << "Good bye!" << std::endl;
				break;
			}
			else {
				std::cout << "Unknown command. Type 'h' or 'help' for usage help." << std::endl;
			}
		}
	}

private:
	void DoRoot()
	{
		int v = 0;
		std::cin >> v;

		state::PlayerIdentifier side;
		if (v == 1) side = state::PlayerIdentifier::First();
		else if (v == 2) side = state::PlayerIdentifier::Second();
		else {
			std::cout << "invalid input" << std::endl;
			return;
		}
		node_ = controller_.GetRootNode(side);
		std::cout << "Current node set to: " << node_ << std::endl;
	}

	void DoInfo()
	{
		if (!node_) {
			std::cout << "Should set node first." << std::endl;
			return;
		}

		std::cout << "Action type: " << GetActionType(node_->GetActionType()) << std::endl;

		if (node_->GetActionType() == mcts::ActionType::kMainAction) {
			std::cout << "Playable hand cards:";
			auto * board_view = node_->GetAddon().consistency_checker.GetBoard();
			node_->GetAddon().action_analyzer.ForEachPlayableCard([&](size_t idx) {
				std::cout << " [" << idx << "] ";
				if (board_view) {
					std::cout << board_view->GetSelfHand()[idx].card_id;
				}
				return true;
			});
			std::cout << std::endl;
		}

		std::cout << "Children:" << std::endl;
		node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
			auto const* edge_addon = node_->GetEdgeAddon(choice);

			if (child.IsRedirectNode()) {
				std::cout << "  " << choice << ": [REDIRECT]" << std::endl;
			}
			else {
				std::cout << "  " << choice << ": [NORMAL] "
					<< child.GetNode() << std::endl;
			}

			if (node_->GetActionType() == mcts::ActionType::kMainAction) {
				auto op = node_->GetAddon().action_analyzer.GetMainOpType(choice);
				std::cout << "    Main Action Op: " << GetOpType(op) << std::endl;
			}

			if (edge_addon) {
				std::cout << "    Chosen time: " << edge_addon->GetChosenTimes() << std::endl;

				double credit_percentage = (double)edge_addon->GetCredit() / edge_addon->GetTotal() * 100;
				std::cout << "    Credit: " << edge_addon->GetCredit() << " / " << edge_addon->GetTotal()
					<< " (" << credit_percentage << "%)"
					<< std::endl;
			}
			return true;
		});
		std::cout << "BoardNodeMap:" << std::endl;
		node_->GetAddon().board_node_map.ForEach([&](mcts::board::BoardView board_view, mcts::builder::TreeBuilder::TreeNode* node) {
			uint64_t total_chosen_time = 0;
			node->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				total_chosen_time += child.GetEdgeAddon().GetChosenTimes();
				return true;
			});
			std::cout << "  " << node << " Chosen time: " << total_chosen_time
				<< std::endl;
			PrintBoardView(board_view, "    ");
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

	void PrintBoardView(mcts::board::BoardView const& view, std::string const& line_prefix) {
		std::cout << line_prefix << "Side: " << GetSideStr(view.GetSide())
			<< std::endl;

		std::cout << line_prefix << "Self resource: "
			<< view.GetSelfCrystal().current << " / " << view.GetSelfCrystal().total
			<< std::endl;

		std::cout << line_prefix << "Self Hero: "
			<< view.GetSelfHero().hp << " / " << view.GetSelfHero().max_hp
			<< " armor: " << view.GetSelfHero().armor
			<< std::endl;

		std::cout << line_prefix << "Self hand cards: ";
		for (auto const& card : view.GetSelfHand()) {
			std::cout << card.card_id << " ";
		}
		std::cout << std::endl;

		std::cout << line_prefix << "Self Minions: ";
		for (auto const& minion : view.GetSelfMinions()) {
			std::cout << minion.hp << "/" << minion.max_hp << " ";
		}
		std::cout << std::endl;

		std::cout << line_prefix << "Opponent resource: "
			<< view.GetOpponentCrystal().current << " / " << view.GetOpponentCrystal().total
			<< std::endl;

		std::cout << line_prefix << "Opponent Hero: "
			<< view.GetOpponentHero().hp << " / " << view.GetOpponentHero().max_hp
			<< " armor: " << view.GetOpponentHero().armor
			<< std::endl;

		std::cout << line_prefix << "Opponent Minions: ";
		for (auto const& minion : view.GetOpponentMinions()) {
			std::cout << minion.hp << "/" << minion.max_hp << " ";
		}
		std::cout << std::endl;
	}

	std::string GetSideStr(state::PlayerSide side) {
		if (side == state::kPlayerFirst) return "kPlayerFirst";
		if (side == state::kPlayerSecond) return "kPlayerSecond";
		if (side == state::kPlayerInvalid) return "kPlayerInvalid";
		return "Unknown!!!";
	}

private:
	ui::AIController controller_;
	mcts::builder::TreeBuilder::TreeNode const* node_;
};

void TestAI()
{
	srand(0);

	Handler handler;
	handler.Start();
}

int main(int argc, char *argv[])
{
	Initialize();
	TestAI();
	return 0;
}
