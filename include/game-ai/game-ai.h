#pragma once

// depends on game-engine
#include "game-engine/game-engine.h"

// game-ai headers
#include "game-ai/board-initializer/board-initializer.h"
#include "mcts.h"
#include "decider.h"
#include "game-ai/json-board-finder/json-board-finder.h"

// game-ai implementation details
#include "tree-impl.h"
#include "traversed-path-recorder-impl.h"
#include "board-node-map-impl.h"
#include "mcts-impl.h"
#include "task-impl.h"
#include "decider-impl.h"

#include "board-initializer/start-board.h"
#include "board-initializer/start-board-impl.h"
#include "board-initializer/json-parser.h"

#include "json-board-finder/json-board-finder-impl.h"