#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/URI.h>
#include <fstream>
#include <iostream>
class ImageAPI {
public:
    std::string GetImage(std::string fen) {
        Poco::URI uri("https://fen2image.chessvision.ai/" + fen);
        Poco::Net::HTTPSClientSession session;
        session.setHost(uri.getHost());
        session.setPort(uri.getPort());
        Poco::Net::HTTPRequest request("GET", uri.toString());
        session.sendRequest(request);
        
        Poco::Net::HTTPResponse response;
        auto& response_stream = session.receiveResponse(response);

        std::string image;
        while (response_stream) {
            image += response_stream.get();
        }
        return image;
    }
};
