#include "telegramAPI.h"
#include "stockfish.h"
#include "updateinfo.h"
#include "data.h"
#include "game.h"

#include <fstream>
#include <memory>
#include <ostream>
#include <regex>
#include <unordered_map>

class Bot {
private:
    bool IsMove(const std::string& text) {
        std::regex move_regex(
            "^/move "
            "([a-h][1-8])"
            "([a-h][1-8])"
            "(?:$|\\s)"
        );
        return std::regex_match(text, move_regex);
    }

public:
    Bot(TelegramAPI* tg_ptr, StockFishEngine* ch_ptr): telegramAPI(tg_ptr), current_game(ch_ptr) {
    }

    void CreateSession() {
        telegramAPI->CreateSession();
    }

    void ReceiveUpdates() {
        updates = std::move(telegramAPI->GetUpdates());
    }

    void SendMessage(long long chat_id, std::string text) {
        telegramAPI->SendMessage(chat_id, text);
    }

    void SendBoard(long long chat_id, const FEN& fen) {
        std::string chessVisionURL = "https://fen2image.chessvision.ai/";

        std::string side = "?pov=white";
        if (current_game.GetSide() == 'b') {
            side = "?pov=black";
        }

        telegramAPI->SendPhoto(chat_id, chessVisionURL + fen.ToString() + side);
    }

    void ExecuteCommands() {
        while (!updates.empty()) {
            const UpdateInfo& current_update = updates.back();
            updates.pop();

            current_game.SetCurGame(current_update.chat_id);

            if (current_update.text == "/new_white") {
                current_game.CreateNewForWhite();
                
            } else if (current_update.text == "/new_black") {
                current_game.CreateNewForBlack();
                current_game.MakeBotMove(current_update.chat_id);
                SendBoard(current_update.chat_id, current_game.GetFen());

            } else if (current_update.text == "/show_board") {
                SendBoard(current_update.chat_id, current_game.GetFen());

            } else if (current_game.IsGameEnded()) {
                SendMessage(current_update.chat_id, "game ended. " + current_game.GetWinner() + " won");

            } else if (IsMove(current_update.text)) {
                Move move(current_update.text);
                 
                bool is_move_valid = current_game.MakePlayerMove(current_update.chat_id, move);
                if (is_move_valid) {

                    if (current_game.IsGameEnded()) {
                        SendMessage(current_update.chat_id, "game ended. " + current_game.GetWinner() + " won");
                        continue;
                    }

                    current_game.MakeBotMove(current_update.chat_id);
                    SendBoard(current_update.chat_id, current_game.GetFen());

                    if (current_game.IsGameEnded()) {
                        SendMessage(current_update.chat_id, "game ended. " + current_game.GetWinner() + " won");
                        continue;
                    }

                } else {
                    SendMessage(current_update.chat_id, "Invalid move. Try again");
                }

            } else {
                SendMessage(current_update.chat_id, "Unknown command");
            }

        }
    }

private:
    std::queue<UpdateInfo> updates;
    std::unique_ptr<TelegramAPI> telegramAPI;
    Game current_game;
};
