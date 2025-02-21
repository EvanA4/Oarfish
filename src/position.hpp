#include <set>
#include <sys/types.h>
#include <iostream>


class Position {
    const std::set<char> PIECES = {
        'P', 'R', 'N', 'B', 'Q', 'K',
        'p', 'r', 'n', 'b', 'q', 'k'
    };

    ulong team_bbs[2] = {0ULL, 0ULL};
    ulong type_bbs[6] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};
    bool whiteTurn;
    short castleRights;
    short passantSq;
    short halfMove;
    short fullMove;

    int zeros(ulong num);
    void parseFEN(std::string &fen);

    public:
        void printBitBoard(ulong bb);
        void printState();
        
        Position(std::string &fen);
};