# Introduction
This is an reinforcement learning pipeline similar to AlphaZero. It is
composed of three key components, namely

1. Self-play: Two agents play with each other, and record the games.
2. Optimizer: Train the neural network according to the game results.
3. Evaluator: Evalute the new trained neural network, and see if it's better.

# Build

```
make
```

# Run

```
mkdir self_play
./alphazero
```

# Results
Although the AlphaZero pipeline proposed by DeepMind seems to be elegant and
promising, it still needs careful tweaks to use on other applications. Some
details of the issues are described below.

## MCTS
The AlphaZero reinforcement learning pipeline uses MCTS to be the policy
strengther, to make sure the self-play produce training data with higher
qualities w.r.t. the nerual network itself.

However, it is spotted that the MCTS tends to choose End-Turn action if
it's not given much computation resource. The reason is simple: the game
tree is not expanded large enough, and the neural network is not yet
trained well enough, so choosing End-Turn action will produce a wrong
idea of 50-50 winning statatistic.

Two ideas are proposed to solve this:
1. Use a pre-trained neural network as the starting point.
2. In MCTS simulation step, use DFS to choose the most promising action.

## Computation Resource
It needs too much computation resource if we targets to a end-user devices.
Both the self-play and evaluation require much computation resource to produce
high-quality results using MCTS.

## Convergence
This is a common issue for EM and many reinforcement learning architecture.
In AlphaZero, the MCTS acts as the policy strengther, to make sure everything
is heading at the correct direction.

However, to make MCTS finish its great and important job, it needs much
computation resource. What makes this even worse is, HearthStone is an
imperfection game, and to tackle this MCTS needs much more computation
resource to average over all random outcomes.
