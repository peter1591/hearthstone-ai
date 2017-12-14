## Introduction
A judgement framework allowing two agents to compete with each other, and make sure they're following the game rules.

## Usage

### Implement agent
Implement your own agent following the interface [IAgent](./include/judge/IAgent.h). Please refer to [agents](../agents) for examples.

```c++
#include "judge/IAgent.h"

class MyAgent : public judge::IAgent
{
  // Implement your own agent
};
```

### Feed start board
There are hidden information like cards held in opponent's hand, but a game state is a state that all hidden information is already determined. So, typically there are multiple possible start boards. A callback function will be invoked by the judger to get a start board. Typically one can do a sampling on all possible hidden information over a predefined distribution.

```c++
#include "state/State.h"

state::State GetStartBoard()
{
  // Return the starting board
}
```

### Invoke judger
```c++
#include "judge/judger.h"

int main()
{
  judge::Judger<> judger;
  MyAgent agent1;
  MyAgent agent2;
  judger.SetFirstAgent(agent1);
  judger.SetSecondAgent(agent2);
  judger.Start(GetStartBoard, agent1, agent2);
  return 0;
}
```

The neural network also uses this judge framework to [generate training dataset](../agents/train/src/GenerateTrainData.cpp).
