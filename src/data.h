#pragma once
#include "fen.h"
#include <unordered_set>
#include <fstream>

//line in game_state.txt
//<chat_id> <fen> <player_side>

struct GameRecord {
    long long chat_id = 0;
    FEN fen;
    char player_side;
    bool is_game_ended = 0;

    bool operator==(const GameRecord& other) const {
        return chat_id == other.chat_id;
    }
};

namespace std {
    template<>
    struct hash<GameRecord> {
        size_t operator()(const GameRecord& record) const {
            return std::hash<long long>{}(record.chat_id);
        }
    };
}

std::istream& operator>>(std::istream& in, GameRecord& record) {
    in >> record.chat_id >> record.fen >> record.player_side >> record.is_game_ended;
    return in;
}

std::ostream& operator<<(std::ostream& os, const GameRecord& record) {
    return os << record.chat_id << ' ' << record.fen << ' ' 
        << record.player_side << ' ' << record.is_game_ended << '\n';
}

class DataBase {
private:
    std::unordered_set<GameRecord> GetDataFromFile() {
        std::unordered_set<GameRecord> records;
        std::ifstream in("game_state.txt");
        if (in.is_open()) {

            GameRecord current_record;

            while (in >> current_record) {
                records.emplace(current_record);
            }
        } else {
            //throw unable to open file
        }
        in.close();
        return records;
    }

    void WriteInFile(const std::unordered_set<GameRecord>& records) {
        std::ofstream out("game_state.txt");
        if (out.is_open()) {
            for (auto& current_record : records) {
                out << current_record;
            }
        } else {
            //throw unable to open file
        }
        out.close();
    }

public: 
    GameRecord GetData(long long chat_id) {
        std::unordered_set<GameRecord> records = GetDataFromFile();
        GameRecord current_record;
        current_record.chat_id = chat_id;

        if (auto it = records.find(current_record); it != records.end()) {
            current_record = *it;
        } else {
            //add throw if not found
        }
        return current_record;
    }

    void UpdateData(const GameRecord current_record) {
        std::unordered_set<GameRecord> records = GetDataFromFile();
        records.erase(current_record);
        records.insert(current_record);
        WriteInFile(records);
    }
};
