#pragma once
#include "fen.h"
#include "data.h"
#include "move.h"
#include "stockfish.h"
#include <algorithm>
#include <istream>
#include <memory>
#include <string>

class Game {
public:
    Game(StockFishEngine* engine_ptr): chessEngine(engine_ptr) {
    }

    void CreateNewForWhite() {
        FEN start_white_fen = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        char player_side = 'w';
        bool is_game_ended = false;

        current_record = GameRecord{current_record.chat_id, start_white_fen, player_side, is_game_ended};
        data_base.UpdateData(current_record);
    }

    void CreateNewForBlack() {
        FEN start_white_fen = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        char player_side = 'b';
        bool is_game_ended = false;

        current_record = GameRecord{current_record.chat_id, start_white_fen, player_side, is_game_ended};
        data_base.UpdateData(current_record);
    }

    bool MakePlayerMove(const long long chat_id, Move move) {
        if (!chessEngine->CheckMove(current_record.fen, move)) {
            return false;
        }

        chessEngine->MakeMoveAndUpdateFen(current_record.fen, move.GetMove());
        current_record.is_game_ended = chessEngine->CheckFenForMate(current_record.fen);

        data_base.UpdateData(current_record);
        return true;
    }

    void MakeBotMove(long long chat_id) {
        std::string best_move = chessEngine->GetBestMove(current_record.fen);

        chessEngine->MakeMoveAndUpdateFen(current_record.fen, best_move);
        current_record.is_game_ended = chessEngine->CheckFenForMate(current_record.fen);

        data_base.UpdateData(current_record);
    }

    void SetCurGame(long long chat_id) {
        current_record = data_base.GetData(chat_id);
    }

    std::string GetFen() {
        return current_record.fen.ToString();
    }

    const char GetSide() const {
        return current_record.player_side;
    }

    std::string GetWinner() const {
        char winner_side = current_record.fen.which_move == 'w' ? 'b' : 'w';
        if (winner_side == current_record.player_side) {
            return std::string("YOU");
        }
        return std::string("BOT");
    }

    bool IsGameEnded() const {
        return current_record.is_game_ended;
    }

private:
    GameRecord current_record;
    DataBase data_base;
    std::unique_ptr<StockFishEngine> chessEngine;
};
