#include "bot.h"

int main() {
    const std::string tg_api_key = "7968488390:AAE2W45e4BVPW_XyJTCJEwKIApUp1MQeruA";
    Bot bot(new TelegramAPI(tg_api_key), new StockFishEngine);
    while (true) {
        bot.ReceiveUpdates();
        bot.ExecuteCommands();
    }
}
