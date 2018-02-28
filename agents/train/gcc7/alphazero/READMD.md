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
