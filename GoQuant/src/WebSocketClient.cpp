#include "../headers/WebSocketClient.h"
#include <iostream>
#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <memory>
#include <thread>
#include <future> // For async

using namespace boost::asio;
using namespace boost::beast;
using namespace rapidjson;

const std::string WS_HOST = "test.deribit.com";
const std::string WS_PORT = "443";
const std::string WS_ENDPOINT = "/ws/api/v2";

// ✅ Pretty-print JSON response
void printPrettyJson(const std::string& jsonStr) {
    rapidjson::Document document;
    document.Parse(jsonStr.c_str());

    if (document.HasParseError()) {
        std::cerr << "Error: Invalid JSON response.\n";
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);  
    document.Accept(writer);

    std::cout << "\n📜 JSON Response:\n" << buffer.GetString() << "\n";
}

// ✅ WebSocketClient Constructor
WebSocketClient::WebSocketClient()
    : ioc(), ssl_context(ssl::context::tlsv12_client), resolver(ioc), is_running(false), is_listening(false) {
    ws = std::make_unique<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>(ioc, ssl_context);
}

// ✅ Destructor ensures WebSocket closure
WebSocketClient::~WebSocketClient() {
    close();
}

// ✅ Asynchronous WebSocket Connection
bool WebSocketClient::connectWeb() {
    try {
        auto const results = resolver.resolve(WS_HOST, WS_PORT);
        auto& tcp_socket = ws->next_layer().next_layer();
        boost::asio::connect(tcp_socket, results);
        ws->next_layer().handshake(ssl::stream_base::client);
        
        ws->set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
            req.set(http::field::user_agent, "Boost.Beast WebSocket Client");
        }));
        
        ws->handshake(WS_HOST, WS_ENDPOINT);

        is_running = true;
        
        // ✅ Run listener asynchronously
        std::async(std::launch::async, &WebSocketClient::listenWithStop, this);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ WebSocket connection error: " << e.what() << std::endl;
        return false;
    }
}

// ✅ Subscribe to Market Data
void WebSocketClient::subscribeToMarketData(const std::string& symbol) {
    if (!is_running) {
        std::cerr << "❌ WebSocket not connected.\n";
        return;
    }

    Document request;
    request.SetObject();
    auto& allocator = request.GetAllocator();
    
    request.AddMember("jsonrpc", "2.0", allocator);
    request.AddMember("method", "public/subscribe", allocator);

    Value params(kObjectType);
    Value channels(kArrayType);
    
    std::string channelName = "ticker." + symbol + ".100ms";
    channels.PushBack(StringRef(channelName.c_str()), allocator);
    params.AddMember("channels", channels, allocator);
    request.AddMember("params", params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    request.Accept(writer);

    try {
        ws->write(net::buffer(buffer.GetString(), buffer.GetSize()));
    } catch (const std::exception& e) {
        std::cerr << "❌ Error sending subscription request: " << e.what() << std::endl;
    }

    // ✅ Start listening in a detached thread
    if (!is_listening) {
        is_listening = true;
        listener_thread = std::thread(&WebSocketClient::listenWithStop, this);
        listener_thread.detach(); // Allow thread to run independently
    }
}

// ✅ Listening Thread for WebSocket Messages
void WebSocketClient::listenWithStop() {
    try {
        while (is_running && is_listening) {
            flat_buffer buffer;
            ws->read(buffer);
            std::string message = buffers_to_string(buffer.data());

            Document responseJson;
            responseJson.Parse(message.c_str());

            if (responseJson.HasParseError()) {
                std::cerr << "❌ Invalid JSON message received.\n";
                continue;
            }

            if (responseJson.HasMember("error")) {
                std::cerr << "❌ WebSocket Error: " << message << std::endl;
                continue;
            }

            printPrettyJson(message);
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Error receiving WebSocket message: " << e.what() << std::endl;
    }
}

// ✅ Stop Listening
void WebSocketClient::stopListening() {
    is_listening = false;
}

// ✅ Close WebSocket Connection Safely
void WebSocketClient::close() {
    if (is_running) {
        is_running = false;
        is_listening = false;
        
        try {
            ws->close(websocket::close_code::normal);
        } catch (const std::exception& e) {
            std::cerr << "❌ Error closing WebSocket: " << e.what() << std::endl;
        }
    }
}
