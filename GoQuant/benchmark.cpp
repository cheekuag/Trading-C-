#include <benchmark/benchmark.h>
#include "headers/Login.h"
#include "headers/DeribitApi.h"

// Global token storage
std::string global_access_token;
std::unique_ptr<DeribitApi> api;

// Function to set up authentication before benchmarks run
static void AuthenticateOnce() {
    if (global_access_token.empty()) {
        std::string client_id = "48ZqOI_P";
        std::string client_secret = "r-UwW3gK-st9p2_NlV9H4NwOuKqhVXt43Befd1fuUfA";
        global_access_token = authenticateUser(client_id, client_secret);
        api = std::make_unique<DeribitApi>("https://test.deribit.com", global_access_token);
    }
}

// Benchmark function for authenticateUser
static void BM_AuthenticateUser(benchmark::State& state) {
    for (auto _ : state) {
        std::string client_id = "48ZqOI_P";
        std::string client_secret = "r-UwW3gK-st9p2_NlV9H4NwOuKqhVXt43Befd1fuUfA";
        std::string token = authenticateUser(client_id, client_secret);
        benchmark::DoNotOptimize(token);
    }
}

// Register authentication once before benchmarking API calls
static void BM_PlaceOrder(benchmark::State& state) {
    AuthenticateOnce();
    for (auto _ : state) {
        api->placeOrder("BTC-PERPETUAL", "buy", 1.0, 50000.0, "benchmark_test");
    }
}

static void BM_CancelOrder(benchmark::State& state) {
    AuthenticateOnce();
    for (auto _ : state) {
        api->cancelOrder("36760655775");
    }
}

static void BM_ModifyOrder(benchmark::State& state) {
    AuthenticateOnce();
    for (auto _ : state) {
        api->modifyOrder("36760655775", 1.0, 51000.0);
    }
}

static void BM_GetOrderBook(benchmark::State& state) {
    AuthenticateOnce();
    for (auto _ : state) {
        api->getOrderBook("BTC-PERPETUAL");
    }
}

static void BM_GetPositions(benchmark::State& state) {
    AuthenticateOnce();
    for (auto _ : state) {
        api->getPositions();
    }
}

// Register the benchmarks
BENCHMARK(BM_AuthenticateUser);
BENCHMARK(BM_PlaceOrder);
BENCHMARK(BM_CancelOrder);
BENCHMARK(BM_ModifyOrder);
BENCHMARK(BM_GetOrderBook);
BENCHMARK(BM_GetPositions);

// Main function for Google Benchmark
BENCHMARK_MAIN();
