#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <cstdlib> // For getenv
#include "headers/Login.h"
#include "headers/WebSocketClient.h"
#include "headers/DeribitApi.h"

// Reads user credentials securely from environment variables
bool readCredentials(std::string& client_id, std::string& client_secret) {
    // Hardcoded credentials for simplicity (should be securely stored in a real application)
    client_id="48ZqOI_P";
    client_secret="r-UwW3gK-st9p2_NlV9H4NwOuKqhVXt43Befd1fuUfA";
    return true;
}

int main() {
    std::string client_id, client_secret;

    // Step 1: Authenticate user
    if (!readCredentials(client_id, client_secret)) {
        return 1; // Exit if credentials are missing
    }

    std::string access_token = authenticateUser(client_id, client_secret);
    if (access_token.empty()) {
        std::cerr << "Authentication failed! No token received." << std::endl;
        return 1;
    }

    std::cout << "✅ User logged in successfully!\n";

    // Step 2: Start WebSocket connection asynchronously
    auto websocket = std::make_unique<WebSocketClient>();
    std::thread ws_thread([&websocket]() {
        if (!websocket->connectWeb()) {
            std::cerr << "WebSocket Connection failed.\n";
        } else {
            std::cout << "✅ WebSocket Initialized\n";
        }
    });
    ws_thread.detach(); // Detach the thread to run independently

    // Step 3: Initialize Deribit API with move semantics
    auto api = std::make_unique<DeribitApi>("https://test.deribit.com", std::move(access_token));

    // Step 4: Show Menu Options
    while (true) {
        std::cout << "\nChoose an action:\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Cancel Order\n";
        std::cout << "3. Modify Order\n";
        std::cout << "4. Get Orderbook\n";
        std::cout << "5. View Positions\n";
        std::cout << "6. Subscribe to Market Data (WebSocket)\n";
        std::cout << "7. Exit from Market Data (WebSocket)\n";
        std::cout << "8. Logout & Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        try {
            switch (choice) {
                case 1: { // Place an order
                    std::string instrument, side;
                    double price;
                    int quantity;

                    std::cout << "Enter instrument (e.g., BTC-PERPETUAL): ";
                    std::cin >> instrument;
                    std::cout << "Enter side (buy/sell): ";
                    std::cin >> side;
                    std::cout << "Enter price: ";
                    std::cin >> price;
                    std::cout << "Enter quantity: ";
                    std::cin >> quantity;

                    api->placeOrder(instrument, side, quantity, price, "");
                    break;
                }

                case 2: { // Cancel an order
                    std::string order_id;
                    std::cout << "Enter order ID to cancel: ";
                    std::cin >> order_id;
                    api->cancelOrder(order_id);
                    break;
                }

                case 3: { // Modify an order
                    std::string order_id;
                    double new_price, amount;
                    std::cout << "Enter order ID to modify: ";
                    std::cin >> order_id;
                    std::cout << "Enter new price: ";
                    std::cin >> new_price;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;

                    api->modifyOrder(order_id, amount, new_price);
                    break;
                }

                case 4: { // Get order book
                    std::string instrument;
                    std::cout << "Enter instrument for order book: ";
                    std::cin >> instrument;
                    api->getOrderBook(instrument);
                    break;
                }

                case 5: // View positions
                    api->getPositions();
                    break;

                case 6: { // Subscribe to market data via WebSocket
                    std::string symbol;
                    std::cout << "Enter symbol to subscribe (e.g., BTC-PERPETUAL): ";
                    std::cin >> symbol;
                    websocket->subscribeToMarketData(symbol);
                    break;
                }

                case 7: { // Stop WebSocket and logout
                    websocket->stopListening();
                    std::cout << "✅ WebSocket listening stopped.\n";
                    break;
                }

                case 8: // Exit application
                    std::cout << "Logging out and closing WebSocket...\n";
                    websocket->close();
                    return 0;

                default: // Handle invalid choice
                    std::cout << "Invalid choice. Try again.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
