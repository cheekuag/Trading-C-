#ifndef DERIBIT_API_H
#define DERIBIT_API_H

#include <string>
#include <memory>
#include <curl/curl.h>
#include "rapidjson/document.h"

class DeribitApi {
public:
    explicit DeribitApi(std::string api_url, std::string auth_token);
    ~DeribitApi();
    
    void placeOrder(std::string instrument_name, std::string side, double amount, double price, std::string label="", bool suppress_output = false);
    void cancelOrder(std::string order_id, bool suppress_output = false);
    void modifyOrder(std::string order_id, double amount,double new_price, bool suppress_output = false);
    void getOrderBook(std::string instrument_name, bool suppress_output = false);
    void getPositions( bool suppress_output = false);
    
private:
    std::string api_url;
    std::string auth_token;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    void sendRequest(std::string endpoint, std::string post_data, bool is_get = false);
};

#endif // DERIBIT_API_H