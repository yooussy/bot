#pragma once
#include <cctype>
#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
struct FEN {
    FEN() = default;

    FEN(std::string str) {
        std::istringstream iss(str);
        std::string temp;
        std::vector<std::string> tokens;
        while (iss >> temp) {
            tokens.push_back(temp);
        }
        board = tokens[0];
        which_move = tokens[1].back();
        castling = tokens[2];
        en_passant = tokens[3];
        halfmove_clock = tokens[4];
        fullmove_number = tokens[5];
    }

    std::string ToString() const {
        return std::string(board + ' ' + which_move + ' ' + castling + ' ' + en_passant
                + ' ' + halfmove_clock + ' ' + fullmove_number);
    }

    std::string MakeBoardString() const {
        std::string res = "`";
        const std::string horizontal_line = "+---+---+---+---+---+---+---+---+\n";
        res += horizontal_line;
        for (int i = 8, j = 0; i > 0; --i) {
            res += "|";
            while (j < board.size() && board[j] != '/') {
                res += " ";
                if (std::isdigit(board[j])) {
                    int digit = board[j] - '0';
                    res += "  |";
                    while (--digit) {
                        res += "   |"; 
                    }
                } else {
                    res += board[j];
                    res += " ";
                    res += "|";
                }
                ++j;
            }
            ++j;
            res += " ";
            res += i + '0';
            res += "\n";
            res += horizontal_line;
        }

        res += "  a   b   c   d   e   f   g   h\n";
        res += "`";
        return res;
    }

    std::string board;
    char which_move;
    std::string castling;
    std::string en_passant;
    std::string halfmove_clock;
    std::string fullmove_number;
};

std::istream& operator>>(std::istream& in, FEN& fen) {
    in >> fen.board >> fen.which_move >> fen.castling >> fen.en_passant >> fen.halfmove_clock
        >> fen. fullmove_number;
    return in;
}

std::ostream& operator<<(std::ostream& os, const FEN& fen) {
    return os << fen.board << ' ' << fen.which_move << ' ' << fen.castling << ' '<< fen.en_passant << ' '
              << fen.halfmove_clock << ' ' << fen.fullmove_number;
}
