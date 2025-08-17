#pragma once
#include <Poco/JSON/Array.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <utility>

#include "updateinfo.h"

class TelegramAPI {
private:
    Poco::Net::HTTPRequest CreateRequest(const std::string& method, const std::string& path, 
            std::optional<std::vector<std::pair<std::string, std::string>>> parameters = std::nullopt) {

        Poco::URI uri(address + path);
        if (parameters) {
            for (auto& p : parameters.value()) {
                uri.addQueryParameter(p.first, p.second);
            }
        }

        return Poco::Net::HTTPRequest(method, uri.toString());
    }

    long long GetOffset() {
        long long offset = 0;
        std::ifstream in("offset.txt");
        if (in.is_open()) {
            in >> offset;
        }
        in.close();

        return offset;
    }

    void SetOffset(long long offset) {
        std::ofstream out("offset.txt");
        if (out.is_open()) {
            out << offset;
        }
        out.close();
    }

    Poco::JSON::Array::Ptr ParseMessagesArray(std::istream& response_stream) {
        Poco::JSON::Parser parser;
        auto reply = parser.parse(response_stream);
        return reply.extract<Poco::JSON::Object::Ptr>()->getArray("result");
    }

public:
    TelegramAPI(const std::string& api_key): address("https://api.telegram.org/bot" + api_key) { 
        CreateSession();
    }

    void CreateSession() {
        Poco::URI uri(address);
        session.setHost(uri.getHost());
        session.setPort(uri.getPort());
    }

    std::queue<UpdateInfo> GetUpdates() {
        std::queue<UpdateInfo> updates;
        long long offset = GetOffset();

        std::vector<std::pair<std::string, std::string>> parameters;

        parameters.emplace_back("offset", std::to_string(offset));
        parameters.emplace_back("timeout", std::to_string(10));

        Poco::Net::HTTPRequest request = CreateRequest("GET", "/getUpdates", parameters);
        session.sendRequest(request);

        Poco::Net::HTTPResponse response;
        auto message_array = ParseMessagesArray(session.receiveResponse(response));

        for (auto it = message_array->begin(); it != message_array->end(); ++it) {
            auto update = it->extract<Poco::JSON::Object::Ptr>();
            offset = update->getValue<long long>("update_id");

            auto message = update->getObject("message");
            std::string text = message->getValue<std::string>("text");

            auto chat = message->getObject("chat");
            long long chat_id = chat->getValue<long long>("id");

            updates.emplace(text, chat_id);
        }
        
        if (!message_array->empty()) {
            SetOffset(++offset);
        }

        return updates;
    }

    void SendMessage(const long long chat_id, std::string message) {
        std::vector<std::pair<std::string, std::string>> parameters;
        parameters.emplace_back(std::make_pair("chat_id", std::to_string(chat_id)));
        parameters.emplace_back(std::make_pair("text", message));

        Poco::Net::HTTPRequest request = CreateRequest("POST", "/sendMessage", parameters);
        session.sendRequest(request);
    }

    void SendPhoto(const long long chat_id, std::string photo) {
        std::vector<std::pair<std::string, std::string>> parameters;
        parameters.emplace_back(std::make_pair("chat_id", std::to_string(chat_id)));
        parameters.emplace_back(std::make_pair("photo", photo));

        Poco::Net::HTTPRequest request = CreateRequest("POST", "/sendPhoto", parameters);
        session.sendRequest(request);
    }

private:
    const std::string address;
    Poco::Net::HTTPSClientSession session;
};
