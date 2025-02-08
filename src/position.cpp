#include <set>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include "position.hpp"


void Position::parseFEN(std::string &fen) {
    int len = fen.length();
    int currIdx = 56; // using Layout 3 for bitboards, FEN starts in top left

    int endPosIdx;
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
            if (currIdx != 8) {
                throw std::runtime_error("Illegal position encoding in FEN string " + fen + ".");
            }
            endPosIdx = i + 1;
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
                    // This isn't even reachable
                    throw std::runtime_error("Failed to parse FEN string " + fen + ".");
            }
            ++currIdx;
        }

        else {
            // Illegal position representation
            std::string errstr =  "Invalid character \"a\" in FEN position encoding.";
            errstr[19] = fen[i];
            throw std::runtime_error(errstr);
        }
    }

    // use endPosIdx to parse the rest of these values:
    // Example: "w KQkq - 0 1"
    whiteTurn = fen[endPosIdx] == 'w';
    if (!whiteTurn && fen[endPosIdx] != 'b') {
        throw std::runtime_error("Invalid player-to-move in FEN string \"" + fen + "\".");
    }

    // short castleRights;
    castleRights = 0;
    int i = endPosIdx + 2;
    while (1) {
        if (fen[i] == ' ') {
            break;
        }

        if (i == len) {
            throw std::runtime_error("FEN string \"" + fen + "\" terminates unexpectedly.");
            break;
        }

        switch (fen[i]) {
            case 'K':
                if (castleRights & 1) {
                    throw std::runtime_error("Illegal repeat of character in castle field.");
                }
                castleRights |= 1;
                break;
            case 'Q':
                if (castleRights & 2) {
                    throw std::runtime_error("Illegal repeat of character in castle field.");
                }
                castleRights |= 2;
                break;
            case 'k':
                if (castleRights & 4) {
                    throw std::runtime_error("Illegal repeat of character in castle field.");
                }
                castleRights |= 4;
                break;
            case 'q':
                if (castleRights & 8) {
                    throw std::runtime_error("Illegal repeat of character in castle field.");
                }
                castleRights |= 8;
                break;
            case '-':
                break;
            default:
                std::string errstr = "Invalid character \"a\" in FEN castle rights.";
                errstr[19] = fen[i];
                throw std::runtime_error(errstr);
                break;
        }

        ++i;
    }

    // short passantSq;
    ++i;
    if (i == len) {
        throw std::runtime_error("FEN string \"" + fen + "\" terminates unexpectedly.");
    }

    if (fen[i] == '-') {
        passantSq = -1;
        i += 2;
    } else {
        if (i + 2 >= len) {
            throw std::runtime_error("FEN string \"" + fen + "\" terminates unexpectedly.");
        }
        passantSq = 8 * (fen[i + 1] - '1') + (fen[i] - 'a');
        i += 3;
    }

    // short halfMove;
    // short fullMove;
    if (sscanf(&fen[i], "%hd %hd", &halfMove, &fullMove) != 2) {
        throw std::runtime_error("FEN string \"" + fen + "\" has invalid en passant, half move, or full move fields.");
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


void Position::printState() {
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

    std::cout << "whiteTurn: " << whiteTurn << std::endl;
    std::cout << "castleRights: 0b"
        << ((castleRights >> 3) & 1)
        << ((castleRights >> 2) & 1)
        << ((castleRights >> 1) & 1)
        << (castleRights & 1) << std::endl;
    std::cout << "passantSq: " << passantSq << std::endl;
    std::cout << "halfMove: " << halfMove << std::endl;
    std::cout << "fullMove: " << fullMove << std::endl;
}


Position::Position(std::string &fen) {
    parseFEN(fen);
    printState();
}