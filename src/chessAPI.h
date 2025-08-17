#pragma once
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <string>

class ChessAPI {
public:
    ChessAPI() = default;

    ChessAPI(const std::string& address): address(address) {
    }

    void CreateSession() {
        Poco::URI uri(address);
        session.setHost(uri.getHost());
        session.setPort(uri.getPort());
    }

    void SendPosition() {
        std::string fen = "2bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3";
        std::string fen1 = "3q1rk1/pp2b1pp/2p2n2/3p1pB1/3P4/1QP2N2/PP1N1PPP/R4RK1 b - - 1 11";

        Poco::URI uri(address);
        uri.addQueryParameter("fen", fen);
        uri.addQueryParameter("multiPv", fen);

        Poco::Net::HTTPRequest request("GET", uri.toString());  

        session.sendRequest(request);
    }

    void ReceivePosition() {
        Poco::Net::HTTPResponse response;
        auto& response_stream = session.receiveResponse(response);
        while (response_stream.peek() != EOF) {
            std::cout << (char)response_stream.get();
        }
    }

private:
    const std::string address;
    Poco::Net::HTTPSClientSession session;
};
