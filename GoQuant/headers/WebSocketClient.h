#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <thread>
#include <atomic>

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    bool connectWeb();
    void subscribeToMarketData(const std::string& symbol);
    void stopListening();  // ⬅️ New function to stop listening
    void close();

private:
    void listenWithStop();  // ⬅️ Modified listening function

    boost::asio::io_context ioc;
    boost::asio::ssl::context ssl_context;
    boost::asio::ip::tcp::resolver resolver;
    std::unique_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> ws;

    std::thread listener_thread;
    std::atomic<bool> is_running;
    std::atomic<bool> is_listening;  // ⬅️ Flag to stop listening
};

#endif
