## Synopsis

UNDER DEVELOPMENT!!!!

This is a game bot for HearthStone.

Your account shall be banned if you use this game bot to play it automatically.

Your account shall *not* be banned if you just refer to the move suggested by this game bot, move your mouse on your own, drag the mouse on your own, and click the mouse on your own.

But, to be on the safe side, I will take no responsbility if your account got banned.

## Motivation

* AlphaGo successfully use MCTS and deep neural networks to beat human on Go.
* Games with hidden information still a big challenge in many ways.
* Give it a try on Hearhstone.

## Components

### Simulation Engine
* There's no HearthStone simulation engine written in C++, so I need to write one.
* Written in C++14/C++17
* One of the design goals is **speed**.
* Use template programming heavily.

### AI Engine
* Monte Carlo tree search
* Consider hidden information
* Shared tree nodes to reduce memory prints and accelerate learning

### Game Board Recognition
* Use the logging feature in HearthStone
* Written in C# since no critical performance issue occurs here.
* Parse the logs to get a picture of the game board.
* Use the C# **yield** to parse the logs in a cleaner way.

### Automatic Play bot
* The automatic play bot is not implemented yet
* Just refer to the move the AI suggested, and do it manually on the game client.

## Installation

1. Install HearthStone on Windows.
2. Enable logging to HearthStone, so we can know what's the board looks like.
3. Open the C# project under 'ui' folder.
4. Run it.

## Contributors

Just me.

## License

The newest GPL license is applied to this project.
