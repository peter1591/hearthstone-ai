# Introduction

General-purpose AI achieves many promising results. Comparing to AI heavily based on hand-crafted rules/features, I'm much more interested in general-purpose and non-rule-based AI methods.

Monte Carlo tree search (aka, MCTS), among to this general-purpose AI category. The AlphaGo from Google's Deepmind successfully combines MCTS with several neural network techniques to efficiently focus on the important tree nodes, and beat human experts.

However, most of the interesting and channelging real-world problems consist of some degree of hidden information and randomness. Unlike to chess Go, it does not have any hidden information, and all moves are deterministic.

Hearthstone, on the other hand, consists a large degree of hidden information and randomness. I started this project to learn MCTS, read papers, think ways to tackle hidden information and randomness, and implement it.

## Demo Videos

### Basic Practice Mode

Opponent is the inn keeper in basic practice mode. Both sides use the same deck.

1. Mage: https://youtu.be/z0I1nM6_k0w
2. Mage: https://youtu.be/L6kr_zJKCQI

### Expert Practice Mode

Opponent is the inn keeper in expert practice mode. Both sides use the same deck.

1. Warlock: https://youtu.be/wLvBlKChFW0
2. Warlock: https://youtu.be/yVX8nTo8o00

# Background

## Hearthstone

### Hidden Information

Hearthstone is a board game which has a large degree of hidden information (e.g., cards in opponent's hand, next card to draw from deck)

### Randomness

Many actions need to choose a random outcome from a **very large set**. For example, summon a random 5-cost minion.

### Simulation Engine

No simulation engine is available elsewhere, so I wrote one by myselves. Here are some facts about my game engine:

1. Written in C++, with some features in C++14/17.
2. Header-only implementation, so no dependency issues.
3. Great performance. E.g., Zero-overhead abstraction, lambda optimization, profile-guided optimization, etc.
4. Framework for client cards. So cards in future expansion can be implemented easily.

## Monte Carlo Tree Search

* Wiki page: https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
* Survey paper
* Parallel MCTS
* MO-MCTS

## Reinforcement Learning

## Neural Network

* Deal with structured data
** Continuous data: health, attack, armor, etc.
** Categorized data: card id, enchantment id, aura id

Libraries:
* Tensorflow
* tiny-dnn: C++, header only, but slow.

# AI

## Monte Carlo Tree Search

Use Monte Carlo tree search to construct the game tree. With several techniques to improve efficiency:

1. Train a neural network to estimate state value.
2. Early cut-off in simulation
3. Use redirect nodes 

## Hidden Information

* Two separated game tree corresponding to the two players. This is the MOMCTS mentioned in [1].
* A tree node maps to a different info of **revealed information** to that player.

## Randomness

A different random outcome should map to a different strategy. For example, if a card says

*put a random 4-cost card to you hand*

Then, you should impose a different strategy according to the card you really get.

In a game tree, a different strategy means a different tree node, which makes the tree quite big.

At least I see three possible solutions:

1. Limit the number of random outcome. So AI will only consider a subset of the random outcome.
2. Enforce to use the same strategy. This is the strategy fusion mentioned in [1], need to apply with care.
3. Use a neural network to separate random outcome into groups.

## Parallel

* virtual loss [6]

## Computation Efficiency

In thoery, Monte Carlo tree search can find the best move only from random playouts, without any hand-crafted rules or expert knowledge. However, this is not possible in practical uses, especially in users' end devices.

The strength of a MCTS player largely depends on two kind of policies:

1. Tree policy: select an action when expanding a node.
2. Default policy: select an action outside the game tree.

These two kinds of policies directly maps to the two neural networks in AlphaGo [3].

### Tree Policy

It is planned to design a neural network to select a good action in a state, but it's still not implemented. Two kinds of tree policies are supported now:

1. Random policy: choose action randomly
2. DFS with state-value function: choose the action with the best state value

### Default Policy

Default policy is used in simulation phase, in which the goal is to have a weak (but fast) player. Several ways to implement:

1. Random policy: choose action randomly
2. Random with hand-crafted rules: with some hard-coded rules to not choose End-Turn action. 
3. DFS with state-value function: choose the action with the best state value

It is worth-mentioning that, early-cutoff [4] is found to be a good approximator. It works as follows:

1. Normally, choose an action according to the default policy
2. WIth a small chance, stop the simulation phase. Guess which side is the winner from that timepoint using state-value function.

### State-Value Function

A state-value function is to know how good the state is. A (deep) neural network is designed for this purpose.

## Redirect node

Typically, many actions can be performed in a different order, without any difference. For example,

1. Attack with the first minion
2. Then, attack with the second minion

### Degree of Flatness

Redirect nodes can be seen as a strategy fusion. A mapping table is needed to find the redirect node.

A naive design is to have a **single mapping table** for the entire game tree. It means all **history** [5] are dropped.

However, we can have a mapping table for **each turn**. It means all actions in that turn is dropped, however, the **overall history** are maintained.

## Reinforcement Learning

* SARSA

Why not reinforcement learning? Want to use computation time for each turn. Maybe combine with MCTS in the future.

# Conclusions

## What Achieved

* For normal users. AI typically uses up to 1-2 GB of memory in 60 seconds thought on a 2016 MacBook Pro.
* Play a good strength for mid-range deck. This means AI can think ahead for several turns, and can gain a good picture of the state by simulations.

## Vision and Future Work

* Debug visualization for MCTS
* AutoML on structured data
* Tensorflow for C++ native support. WIth optional compile-time third-library support.
* A library for MCTS. MCTS needs several carefully tweaks to be computationally efficient. How to make it general? easier?

# Appendix

## About Me

I'm really glad to start this project. With only a light background on Game AI, reading papers is surely the first thing. Understand the state-of-the-art techniques is always an exciting thing.

1. Start coding in my fourth grade in dbase III, QBasic, VB4.0, etc.
2. Banchelor in computer science network department in National Chiao Tung University.
3. Ph.D. in computer vision in National Chiao Tung University.
4. Software enginner in Synology (as my millitary service) on synchronization frameworks.
5. Currently a software enginner in Google.

But what interests me is always my side projects!

1. Game bot for Maplestory by reverse enginnering, modify register values, etc.
2. Game AI bot for many three-connect games
3. Game bot by sending crafted packets to server by reverse enginnering Flash games.
4. Now, this AI for Hearthstone.

My favorite programming language is C++, especially modern C++! It's so good to control everything, and zero-overhead abstraction the best feature I like. An orgnaized code with excellent performance, what can you ask more?


## References

[1] mentioned strategy fusion in MCTS. also mentioned MOMCTS
[2] the ai competition
[3] AlphaGo paper
[4] simluation early cutoff
[5] MCTS survey. Mentioned 'history'
[6] MCTS virtual loss
