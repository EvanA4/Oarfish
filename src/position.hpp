#include <set>
#include <iostream>

typedef unsigned long long ulong;


class Position {
    public:
        ulong team_bbs[2] = {0ULL, 0ULL};
        ulong type_bbs[6] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};

    private:
        const std::set<char> PIECES = {
            'P', 'R', 'N', 'B', 'Q', 'K',
            'p', 'r', 'n', 'b', 'q', 'k'
        };
        void fillBitboards(std::string &fen);

    public:
        void printBitBoard(ulong bb);
        void printBitBoards();
        Position(std::string &fen);
};