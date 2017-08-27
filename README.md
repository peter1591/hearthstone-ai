## Synopsis

UNDER DEVELOPMENT!!!!

The goal for this project is to design a game bot for HearthStone.

## Motivation
* AlphaGo successfully combines MCTS and deep neural networks to beat human on Go.
* Games with hidden information still a big challenge in many ways.
* Give it a try on Hearhstone.

## Components

### Simulation Engine
* There's no HearthStone simulation engine written in C++, so I decided to write one.
* Header-only implementation. No need to compile anything!
* Written in C++14/C++17.
  * Need gcc 7.0+ or Visual Studio 2017 Preview 2.1+ to compile.
  * Heavily use template programming.
* The most important goal is **speed**.
* Profile-guided optimization is integrated in build script.
* So glad to be used in AAIA'17 Data Mining Challenge: Helping AI to Play Hearthstone (https://knowledgepit.fedcsis.org/mod/page/view.php?id=1022)

### AI Engine
* Monte Carlo tree search
* Use Multiple-Observer MCTS to handle hidden information
* Share tree nodes for identical boards

### Game Board Recognition
* Use the logging feature in HearthStone
* Written in C# since no critical performance issue occurs here.
* Parse the logs to get a picture of the game board.
* Use the C# coroutine to parse the logs in a cleaner way.

### Automatic Play bot
* The automatic play bot is not implemented yet
* Just refer to the move the AI suggested, and do it manually on the game client.

## Installation
1. Install HearthStone on Windows.
2. Enable logging to HearthStone, so we can know what's the board looks like.
3. Open the C# project under 'ui' folder.
4. Run it.

## Contributors

Just me. Any idea/help is always welcomed.

## License

The newest GPL license is applied to this project.
