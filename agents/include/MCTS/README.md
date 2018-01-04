# Monte Carlo Tree Search

This is the Monte Carlo tree search module, which explore and expand the game tree in a monte-carlo fashion.

## Features
* Tackle **hidden information** using information set. Method proposed in the paper [Information Set Monte Carlo Tree Search](http://eprints.whiterose.ac.uk/75048/1/CowlingPowleyWhitehouse2012.pdf)
* Tackle randomness by creating redirect nodes
* Share identical game state in one single tree node
  * For example, if two actions can be swapped without any notable difference, the corresponding two tree nodes can in fact map to the one single node.
* Parallel Monte Carlo tree search

## Framework Architecture
The MCTS algorithm are separated from different application-specific settings/configurations. The application-specific settings are collected in the [config header](./Config.h) file, and all related classes are collected in the [policy](./policy) namespace.

## Thread Safety
* Support multi-thread environment
* Support virtual loss to improve multi-thread performance

## Reference
1. [Information Set Monte Carlo Tree Search](http://eprints.whiterose.ac.uk/75048/1/CowlingPowleyWhitehouse2012.pdf)
