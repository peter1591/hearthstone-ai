CFLAGS_PROJECT  = -std=c++11
#CFLAGS_PROJECT  += -O2 -Wall -Wpedantic -Wextra -g -static
CFLAGS_PROJECT  += -Wall -Wpedantic -Wextra -g -static

# let player/opponent choose where to put a minion?
#CFLAGS_PROJECT += -DCHOOSE_WHERE_TO_PUT_MINION

# checks if a player/opponent's turn introduce any randoms
CFLAGS_PROJECT += -DDEBUG

# check if move can re-applied determinstically
CFLAGS_PROJECT += -DDEBUG_SAVE_BOARD
