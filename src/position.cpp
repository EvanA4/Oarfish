#include <set>
#include <iostream>
#include "position.hpp"


void Position::fillBitboards(std::string &fen) {
    int len = fen.length();
    int currIdx = 56; // using Layout 3 for bitboards, FEN starts in top left

    for (int i = 0; i < len; ++i) {

        // if empty space(s), add to currIdx
        if (fen[i] >= '1' && fen[i] <= '9') {
            currIdx += fen[i] - '0';
        }

        // if end of rank, subtract 8
        else if (fen[i] == '/') {
            currIdx -= 16;
        }

        // if end of pieces str, end loop
        else if (fen[i] == ' ') {
            break;
        }

        // if a piece, get team, type, and then add
        else if (PIECES.find(fen[i]) != PIECES.end()) {
            bool isWhite = fen[i] < 'a';
            team_bbs[isWhite] |= (1ULL << currIdx);

            switch (isWhite ? fen[i] + ('a' - 'A') : fen[i]) {
                case ('p'):
                    type_bbs[0] |= (1ULL << currIdx);
                    break;
                case ('r'):
                    type_bbs[1] |= (1ULL << currIdx);
                    break;
                case ('n'):
                    type_bbs[2] |= (1ULL << currIdx);
                    break;
                case ('b'):
                    type_bbs[3] |= (1ULL << currIdx);
                    break;
                case ('q'):
                    type_bbs[4] |= (1ULL << currIdx);
                    break;
                case ('k'):
                    type_bbs[5] |= (1ULL << currIdx);
                    break;
                default:
                    // ==========================THROW ERROR
                    break;
            }
            ++currIdx;
        }

        else {
            // =======================THROW ERROR
        }
    }
}


void Position::printBitBoard(ulong bb) {
    for (int i = 7; i >= 0; --i) {
        unsigned char rank = (bb >> (8 * i)) & 255ULL;
        for (int j = 0; j < 8; ++j) {
            std::cout << ((rank >> j) & 1) << ' ';
        }
        std::cout << std::endl;
    }
}


void Position::printBitBoards() {
    std::cout << "White pieces:" << std::endl;
    printBitBoard(team_bbs[0]);
    std::cout << std::endl << "Black pieces:" << std::endl;
    printBitBoard(team_bbs[1]);
    std::cout << std::endl;

    std::string types[6] = {"Pawn", "Rook", "Knight", "Bishop", "Queen", "King"};
    for (int i = 0; i < 6; ++i) {
        std::cout << types[i] << " pieces:" << std::endl;
        printBitBoard(type_bbs[i]);
        std::cout << std::endl;
    }
}


Position::Position(std::string &fen) {
    fillBitboards(fen);
    printBitBoards();
}