## Introduction
The game engine for the card game [Hearthstone](https://playhearthstone.com).

## Features

### Header-only Implementation
* No dependency issue.
* (Free/Automatically) whole program optimization

### Performance
Use template programming to move workload to compile-time as much as possible. With C++14/17 features like [std::variant](http://en.cppreference.com/w/cpp/utility/variant), [SFINAE](http://en.cppreference.com/w/cpp/language/sfinae) and much more, we can write structured code with zero overhead.

## Components
1. [Game state](./include/state) maintains the underlying game state.
2. [Flow control](./include/FlowControl) makes sure the game follows the rule.
3. [Cards](./include/Cards) implements all the cards.
4. [Decks](./include/decks) collects all types of deck.

## Usage
```c++
#include <state/State.h>
#include <FlowControl/FlowController.h>

class ActionParameterGetter; // Callback to get the spell/battlecry target, minion put location, etc.
class RandomGenerator; // Callback to get a random number for all randomness in game flow

void test()
{
  state::State game_state;
  ActionParameterGetter action_cb;
  RandomGenerator random_cb;
  FlowControl::FlowContext context(random_cb, action_cb);
  Prepare(); // Prepare hero/hero-power/deck/hand...
  FlowControl::FlowController controller(game_state, context);
  controller.PlayCard(0); // Play the first card
}
```
Refer to [e2e test](./test) for concrete examples.

## Troubleshooting

### Option /bigobj
Enable the [/bigobj](https://technet.microsoft.com/en-us/library/ms173499(v=vs.90).aspx/html) option for Visual Studio.

### Too slow (re)compilation
This is a header-only implementation. If the compiler is not smart enough, all the headers have to be re-compiled each time. A programmer can always choose to move the implementation detail to another compilation unit.

Refer to the [card-dispatcher](./test/e2e_card_dispatcher.cpp) compilation unit for an example to separate out implementation details for [client cards](./include/Cards) and [flow controller](./include/FlowControl).
