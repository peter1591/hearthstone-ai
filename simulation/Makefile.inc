CFLAGS_PROJECT  = -std=c++11
CFLAGS_PROJECT  += -O2 -Wall -Wpedantic -Wextra -g -static

# let player/opponent choose where to put a minion?
CFLAGS_PROJECT  += -DCHOOSE_WHERE_TO_PUT_MINION

# checks if a player/opponent's turn introduce any randoms
#CFLAGS_PROJECT  += -DDEBUG
