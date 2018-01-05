# Agents based on Monte Carlo Tree Search

This is an agent for the Hearthstone [judge framework](../../judge). This agent choose the most promising using [Monte Carlo Tree Search](../MCTS) with a neural network.

## Features

* **Any time**: This agent can be stopped at any time, and return the most promising action so far.
* **Learnable**: A neural network is used during the tree expansion. This neural network can be improved from [training](./train).
