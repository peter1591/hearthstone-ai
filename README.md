## Introduction

The is an AI for the card game HearthStone which originally motivated by AlphaGo! This work combines Monte Carlo tree search (with extensions for imperfection game), deep neural network, and a high-performance game engine.

Compete with Mage in basic practice mode. Running on Macbook Pro. AI can easily beat innkeeper (8 = 0).
[![Video](https://img.youtube.com/vi/L6kr_zJKCQI/0.jpg)](http://www.youtube.com/watch?v=L6kr_zJKCQI)

Compete with Warlock in expert practice mode. Running on Macbook Pro.
[![Video](https://img.youtube.com/vi/wLvBlKChFW0/0.jpg)](http://www.youtube.com/watch?v=wLvBlKChFW0)

## Motivation
* AlphaGo successfully combines MCTS and deep neural networks to beat human on Go.
* Games with hidden information are still a big challenge in many ways.
* Give it a try on Hearhstone.

## Modules

### [Game engine](./engine)
* Header-only implementation. No dependency. No need to compile anything!
* Use template programming intensively for higher performance.

### [Judgement framework](./judge)
* A judgement framework allowing two agents to compete with each other.

### [MCTS Agent](./agents/include/agents)
* [Monte Carlo tree search](./agents/include/MCTS)
  * Use Multiple-Observer MCTS to handle hidden information
  * Share tree nodes for identical boards
* Combine with a [neural network](./agents/train/tensorflow)
  * Act as a policy network to choose the promising steps with higher probabilities
  * Also act as a default network to play the game in simulation phase
  * Guess the game result for early cutoff

### Neural Network

Use [TensorFlow](https://www.tensorflow.org) for training and prediction. The neural network model is defined in [`model.py`](./agents/train/tensorflow/model.py). A neural network model can be trained and integrated into the [MCTS agent](./agents/include/agents) by the following steps:

1. Prepare [training data](./agents/train/gcc7/generate_train_data).
2. [Train](./agents/train/tensorflow) the model using Tensorflow.
3. Save andfreeze the model.
4. Set the model path to MCTS agent.

A simple example shows the neural network can *greatly* boost MCTS play strength:
  * A mid-level person knows the *arcane missiles* should generally not be played in the first turn.
  * If using random default policy, it takes more than 300k iterations (8G+ RAM) to realize this.
  * If using neural network as default policy, it only takes < 15k iterations (less than 5 seconds) to realize this.

### [Game Board Recognition](./ui/build/vs2017/HearthstoneAI/LogWatcher)
* Use the logging feature in HearthStone
* Written in C# since no critical performance issue occurs here.
* Parse the logs to get a picture of the game board.
* Use the C# coroutine to parse the logs in a cleaner way.

### [Graphical User Interface](./ui)
* Integrate everything in one piece.
* Automatically show a suggestion move as you play the game. See videos like [this](https://youtu.be/L6kr_zJKCQI) for a demo.

## Installation
1. Install HearthStone on Windows.
2. Enable logging to HearthStone, so we can know what's the board looks like.
3. Open the C# project under [this folder](./ui/build/vs2017).
4. Compile and run it.

## Future Works

### Neural Network Improvements

The goal of the neural network is to guess who is going to win this game, by looking at only the current board. Several improvements could be done:

1. Take history data into account: secret cards, played cards, etc.
2. Take hand cards into account.
3. Take card id into account. Currently, only HP/Max-HP/Attack are considered.
4. Take cards in deck into account.

Hope we can have a better accuracy than current result (~79%, which also aligned to the result of AAIA'17 Data Mining Challenge: Helping AI to Play Hearthstone (https://knowledgepit.fedcsis.org/mod/page/view.php?id=1022)).

I have tried to embedding the card id to encode the battlecry and deathrattle features for each different card. Maybe we need to find a better way to generate game data automatically, so the neural network can learn the embeddings separately and hopefully more accurately.

### Balance Between Wide and Deep

Why wide? Due to randomness, the branch factor is quite large (~4000 when drawing a card). So, there are many tree nodes in the game tree.

Why deep? The neural network by itself are not strength enough. We need to think ahead more steps to overcome the weakness in simulation.

In a naive implementation of MCTS, all the children nodes must be expanded before we use UCB formula to choose a child node and continues in selection stage. Few ideas here:
1. A fixed possibility to continue in selection stage. Even not all children are expanded.
2. A dynamic possibility based on **rest of thinking time** and **current expansion progress**.

### Share information between nodes

Even if there are only one card is different, we still need two tree nodes. Otherwise, we will fuse the strategy decision in Monte-Carlo tree search. However, this does *not* means that, we cannot share information between nodes. On the contrary, AMAF (all-move-as-first) and RAVE (rapid action value estimation) are based on this basic idea.

### Automatic Play bot

Right now, Just refer to the move the AI suggested, and do it manually on the game client.

## Demo Videos
* First demo video is on!!! https://youtu.be/z0I1nM6_k0w
* Another demo video with higher quality: https://youtu.be/L6kr_zJKCQI
* First demo video with expert warlock: https://youtu.be/wLvBlKChFW0
* Another demo video played with expert warlock innkeeper: https://youtu.be/yVX8nTo8o00

## Contribution

Just me. Any idea/help is always welcomed.

## License

The newest GPL license is applied to this project.

Some third party libraries are used, please also obey to their licenses.
