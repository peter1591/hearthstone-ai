## Introduction
A judgement framework allowing two agents to compete with each other, and make sure they're following the game rules.

## Usage

## Implement agent
Implement your own agent following the interface [IAgent](./include/judge/IAgent.h). Please refer to [agents](../agents) for examples.

### Invoke judger
```c++
#include "judge/judger.h"

class MyAgent : public judge::IAgent
{
  // Implement your own agent
};

state::State GetStartBoard()
{
  // Return the starting board
}

int main()
{
  judge::Judger<> judger;
  MyAgent agent1;
  MyAgent agent2;
  judger.SetFirstAgent(agent1);
  judger.SetFirstAgent(agent2);
  judger.Start(GetStartBoard, agent1, agent2);
  return 0;
}
```

The neural network also uses this judge framework to [generate training dataset](../agents/train/src/GenerateTrainData.cpp).
