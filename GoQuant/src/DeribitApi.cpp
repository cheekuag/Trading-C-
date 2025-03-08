#include "../headers/DeribitApi.h"
#include <iostream>
#include <sstream>  // Required for std::ostringstream
#include <iomanip>
#include <rapidjson/document.h>  // Include for Document
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;  // Required for Document, Value, etc.
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// Function to pretty-print a JSON string
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

// Function to pretty-print a JSON string
void printPrettyJson( std::string& jsonStr) {
    rapidjson::Document document;
    document.Parse(jsonStr.c_str());

    if (document.HasParseError()) {
        std::cerr << "Error: Invalid JSON response.\n";
        return;
    }

    // Pretty formatting
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);  // Use 4 spaces for indentation
    document.Accept(writer);

    std::cout << "\n📜 JSON Response:\n" << buffer.GetString() << "\n";
}



DeribitApi::DeribitApi(std::string api_url, std::string auth_token) : api_url(std::move(api_url)), auth_token(std::move(auth_token)) {std::cout<<"Deribit     "<<auth_token<<std::endl;}

DeribitApi::~DeribitApi() {}

size_t DeribitApi::writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


void DeribitApi::sendRequest(std::string endpoint, std::string post_data, bool is_get) {
    CURL* curl = curl_easy_init();
    if (!curl) return ;
    
    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, (api_url + endpoint).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    if (!is_get) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    }
    
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    printPrettyJson(response);
}

void  DeribitApi::placeOrder(std::string instrument_name, std::string side, double amount, double price, std::string label, bool suppress_output ) {
    std::cout<<"placeorder"<<std::endl;

    std::string endpoint = "/api/v2/private/" + side +
                           "?amount=" + std::to_string(int (amount) ) +
                           "&instrument_name=" + instrument_name +
                           "&type=limit" +  // Always use "limit"
                           "&price=" + std::to_string(price); // Include price

    std::cout<<endpoint<<std::endl;
    if (!label.empty()) {
        endpoint += "&label=" + label;
    }
    
    sendRequest(endpoint, "", true);
}


void DeribitApi::cancelOrder(std::string order_id, bool suppress_output ) {
    if (order_id.empty()) {
        std::cerr << "Error: order_id is empty!" << std::endl;
         std::cout<<("{\"error\": \"order_id is required\"}")<<std::endl;
    }

    CURL* curl = curl_easy_init();  // Create a CURL handle
    if (!curl) {
        std::cerr << "Error: Failed to initialize CURL" << std::endl;
         std::cout<<("{\"error\": \"CURL init failed\"}")<<std::endl;
    }

    char* escaped_order_id = curl_easy_escape(curl, order_id.c_str(), order_id.length());
    if (!escaped_order_id) {
        curl_easy_cleanup(curl);  // Clean up CURL instance
        std::cerr << "Error: Failed to escape order_id" << std::endl;
         std::cout<<("{\"error\": \"Failed to escape order_id\"}")<<std::endl;
    }

    std::string endpoint = "/api/v2/private/cancel?order_id=" + std::string(escaped_order_id);
    sendRequest(endpoint,"",true);
}

void DeribitApi::modifyOrder(std::string order_id, double amount,double new_price, bool suppress_output ) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << new_price;

    // Construct the query parameters
    std::string post_data = "/api/v2/private/edit?amount=" + std::to_string(amount) +
                            "&order_id=" + order_id + 
                            "&price=" + oss.str();
     sendRequest(post_data, "",true);
}

void DeribitApi::getOrderBook(std::string instrument_name, bool suppress_output ) {
     sendRequest("/api/v2/public/get_order_book?instrument_name=" + instrument_name, "", true);
}

void DeribitApi::getPositions( bool suppress_output ) {
     sendRequest("/api/v2/private/get_positions", "", true);
}
