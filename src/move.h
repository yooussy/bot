#pragma once
#include <memory>
#include <string>

class Move {
public:
    Move(std::string move): move(move.substr(6)) {
        // move = "/move e2e4";
        //          0123456789
    }

    std::string GetMove() const {
        return move;
    }
    
private:
    std::string move;
};
