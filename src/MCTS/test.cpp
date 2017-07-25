#include "FlowControl/FlowController-impl.h"

#include <iostream>
#include <chrono>

#include "Cards/PreIndexedCards.h"
#include "TestStateBuilder.h"
#include "UI/AIController.h"

void Initialize()
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
		Initialize();

		auto start_board_getter = []() -> state::State {
			return TestStateBuilder().GetState();
		};

		while (std::cin) {
			std::string cmd;
			std::cout << "Command: ";
			std::cin >> cmd;

			if (cmd == "h" || cmd == "help") {
				std::cout << "Commands: " << std::endl
					<< "h or help: show this message" << std::endl
					<< "s or start (secs): to run for a specified seconds" << std::endl
					<< "root (1 or 2): set node to root node of player 1 or 2" << std::endl
					<< "info: show info for selected node" << std::endl
					<< "node (addr): set node to specified address." << std::endl
					<< "q or quit: quit" << std::endl;
			}
			else if (cmd == "s" || cmd == "start") {
				int secs = 0;
				std::cin >> secs;

				auto start = std::chrono::steady_clock::now();
				auto start_i = controller_.GetStatistic().GetSuccededIterates();
				controller_.Run(secs, start_board_getter);
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
				std::cin >> std::hex >> v;
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
		std::cout << "Children:" << std::endl;
		node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
			auto const* edge_addon = node_->GetEdgeAddon(choice);
			std::cout << "  " << choice << ": "
				<< GetChildNodeType(child) << " "
				<< child.GetNode() << std::endl;

			if (node_->GetActionType() == mcts::ActionType::kMainAction) {
				auto op = node_->GetAddon().action_analyzer.GetMainOpType(choice);
				std::cout << "    Main Action Op: " << GetOpType(op) << std::endl;
			}

			if (edge_addon) {
				std::cout << "    Chosen time: " << edge_addon->chosen_times << std::endl;
				std::cout << "    Credit: " << edge_addon->credit << " / " << edge_addon->total << std::endl;
			}
			return true;
		});
		std::cout << "BoardNodeMap:" << std::endl;
		node_->GetAddon().board_node_map.ForEach([&](mcts::board::BoardView board_view, mcts::builder::TreeBuilder::TreeNode* node) {
			std::cout << "  " << node << std::endl;
			return true;
		});
	}

	std::string GetChildNodeType(mcts::selection::ChildType const& child) {
		if (child.IsRedirectNode()) return "[REDIRECT]";
		else return "[NORMAL]";
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

private:
	ui::AIController controller_;
	mcts::builder::TreeBuilder::TreeNode const* node_;
};

int main(void)
{
	Handler handler;
	handler.Start();
	return 0;
}
