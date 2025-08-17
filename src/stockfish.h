#pragma once
#include <boost/process.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <boost/process/pipe.hpp>
#include <iostream>
#include <string>
#include <regex>
#include "fen.h"
#include "move.h"

using namespace boost::process;

class StockFishEngine {
private:
    void SendCommand(std::string command) {
        engine_in << command << '\n';
        engine_in.flush();
    }

    std::string WaitForResponse(const std::string& keyword) {
        std::string line;
        auto start = std::chrono::steady_clock::now();
        
        while (true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (elapsed.count() >= 1) {
                return "";
            }

            if (std::getline(engine_out, line) && line.find(keyword) != std::string::npos) {
                return line;
            }
        }
    }

    std::vector<std::string> GetValidMoves() {
        std::string line;
        std::regex pattern(R"(^[a-h][1-8][a-h][1-8]:.*)");
        std::vector<std::string> res; 
        while (true) {
            getline(engine_out, line);
            if (line.find("Nodes") != std::string::npos) {
                break;
            }
            if (std::regex_match(line, pattern)) {
                res.push_back(line.substr(0, 4));
            }
        }
        return res;
    }

public:
    StockFishEngine(): engine("stockfish", std_in < engine_in, std_out > engine_out) {
        SendCommand("setoption name Skill Level value 0");
    }

    ~StockFishEngine() {
        engine.terminate();
    }

    void MakeMoveAndUpdateFen(FEN& fen, const std::string& move) {
        SendCommand("position fen " + fen.ToString() + " moves " + move);
        SendCommand("d");
        std::string new_fen = WaitForResponse("Fen:");
        fen = new_fen.substr(5);
    }

    bool CheckMove(const FEN& fen, const Move& move) {
        SendCommand("position fen " + fen.ToString());
        SendCommand("go perft 1");
        std::vector<std::string> valid_moves = GetValidMoves();
        for (auto& s : valid_moves) {
            if (s == move.GetMove()) {
                return true;
            }
        }
        return false;
    }

    std::string GetBestMove(const FEN& fen) {
        SendCommand("position fen " + fen.ToString());
        SendCommand("go depth 1");
        std::string bestmove_line = WaitForResponse("bestmove");

        //bestmove e2e4 ponder e7e5
        //0123456789012
        std::string bestmove = bestmove_line.substr(9, 4);
        return bestmove;
    }

    bool CheckFenForMate(FEN& fen) {
        SendCommand("position fen " + fen.ToString());
        SendCommand("go depth 1");
        std::string line = WaitForResponse("info depth");

        if (line.find("mate 0") != std::string::npos) {
            return true;
        }
        return false;
    }

private:
    opstream engine_in;
    ipstream engine_out;
    child engine;
};
