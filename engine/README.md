## Introduction
The game engine for the card game [Hearthstone](https://playhearthstone.com) written in C++17. Used in [AAIA'17 Data Mining Challenge: Helping AI to Play Hearthstone](https://knowledgepit.fedcsis.org/mod/page/view.php?id=1022) to generate training/validation datasets.

## Features

### Header-only implementation
* No dependency
* (Free/Automatically) whole program optimization

### Modern C++
Use template programming to move workload to compile-time as much as possible. With C++14/17 features like [std::variant](http://en.cppreference.com/w/cpp/utility/variant), [SFINAE](http://en.cppreference.com/w/cpp/language/sfinae) and much more, we can write structured code with zero overhead.

## Components
1. [Game state](./include/state) maintains the underlying game state.
2. [Flow control](./include/FlowControl) makes sure the game follows the rule.
3. [Cards](./include/Cards) implements all the cards.
4. [Decks](./include/decks) collects all types of deck.

## Usage

### Action parameter callback
According to Hearthstone's rule, a game is divided into several **turns**, and a player can perform several **main actions** at each turn. There are only four **main actions**:

1. Play a card
2. Attack
3. Use hero power
4. End turn

In a **main action**, it is sometimes needed to provide further **action parameters** like choose a spell target or choose where to put the minion. To provide these parameters, we need to implement a callback first.

```c++
#include "FlowControl/IActionParameterGetter.h"

class MyActionParameterGetter : public FlowControl::IActionParameterGetter
{
  // implementation
};
```

### Randomness callback
Hearthstone is a game with a lot of randomness. All the random outcomes will be fetched from the provided callback. Once you fix the random outcomes from your random callback, this becomes a deterministic game.

```c++
#include "state/IRandomGenerator.h"

class MyRandomGenerator : public state::IRandomGenerator
{
  // implementation
};
```

### Final step! 
```c++
#include "FlowControl/FlowController.h"

void test()
{
  state::State game_state;
  MyActionParameterGetter action_cb;
  MyRandomGenerator random_cb;
  FlowControl::FlowContext context(random_cb, action_cb);
  Prepare(); // Prepare hero/hero-power/deck/hand...
  FlowControl::FlowController controller(game_state, context);
  controller.PlayCard(0); // Play the first card
}
```
Refer to [e2e test](./test) for a full examples.

## Build
This library uses some language features in the latest C++17, compiler newer than gcc 7.0+ or Visual Studio 2017 Preview 2.1+ is needed. Please refer to the [build folder](./build) for working examples.

## Troubleshooting

### Option /bigobj
Enable the [/bigobj](https://technet.microsoft.com/en-us/library/ms173499(v=vs.90).aspx/html) option for Visual Studio.

### Too slow (re)compilation
This is a header-only implementation. If the compiler is not smart enough, all the headers have to be re-compiled each time. A programmer can always choose to move the implementation detail to another compilation unit.

Refer to the [card-dispatcher](./test/e2e_card_dispatcher.cpp) compilation unit for an example to separate out implementation details for [client cards](./include/Cards) and [flow controller](./include/FlowControl).
