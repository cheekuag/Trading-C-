#include "../headers/Login.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <memory>
#include <sstream>
#include <iomanip>

using namespace boost::beast;
using namespace boost::asio;
using namespace rapidjson;
namespace ssl = boost::asio::ssl;

// Deribit API Constants
const std::string API_HOST = "test.deribit.com";
const std::string API_PORT = "443";
const std::string API_LOGIN_ENDPOINT = "/api/v2/public/auth";

// Function to authenticate with Deribit API and return access token
std::string authenticateUser( std::string& client_id,  std::string& client_secret) {
    try {
        io_context ioc;
        ssl::context ctx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> stream(ioc, ctx);

        // Resolve API host
        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(API_HOST, API_PORT);
        connect(stream.next_layer(), results.begin(), results.end());

        // Perform SSL handshake
        stream.handshake(ssl::stream_base::client);

        // Construct GET request URL
        std::string request_url = API_LOGIN_ENDPOINT + "?client_id=" + client_id +
                                  "&client_secret=" + client_secret +
                                  "&grant_type=client_credentials";

        // Prepare HTTP GET request
        http::request<http::string_body> req(http::verb::get, request_url, 11);
        req.set(http::field::host, API_HOST);
        req.set(http::field::user_agent, "Boost.Beast Client");

        // Send request
        http::write(stream, req);

        // Receive response
        flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Convert response body to string
        std::string responseBody = buffers_to_string(res.body().data());

        // Print raw response for debugging
        std::cout << "Raw Response: " << responseBody << std::endl;

        // Parse JSON response
        Document responseJson;
        responseJson.Parse(responseBody.c_str());

        // Validate JSON response
        if (responseJson.HasParseError()) {
            std::cerr << "Error: JSON Parse Error - " << responseJson.GetParseError() << std::endl;
            return "";
        }

        if (!responseJson.HasMember("result") || !responseJson["result"].HasMember("access_token")) {
            std::cerr << "Error: Authentication failed, missing access_token" << std::endl;
            return "";
        }

        std::string access_token = responseJson["result"]["access_token"].GetString();
        std::cout << "Login successful! Access Token: " << access_token << std::endl;
        return std::move(access_token);  
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}
