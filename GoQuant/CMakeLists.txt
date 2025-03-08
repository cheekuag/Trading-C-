cmake_minimum_required(VERSION 3.14)
project(DeribitTradingSystem)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system coroutine thread)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(benchmark REQUIRED)  # Google Benchmark

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/third_party/rapidjson/include)
include_directories(${CURL_INCLUDE_DIRS})

# Source files
set(SOURCES 
    main.cpp
    src/Login.cpp
    src/WebSocketClient.cpp
    src/DeribitApi.cpp
)

# Header files
set(HEADERS
    headers/Login.h
    headers/WebSocketClient.h
    headers/DeribitApi.h
)

# Create main executable
add_executable(DeribitTradingSystem ${SOURCES} ${HEADERS})

# Link required libraries
target_link_libraries(DeribitTradingSystem 
    PRIVATE Boost::system 
    PRIVATE Boost::coroutine 
    PRIVATE Boost::thread
    PRIVATE OpenSSL::SSL 
    PRIVATE OpenSSL::Crypto
    PRIVATE ${CURL_LIBRARIES}
)

# --- Google Benchmark Setup ---
# Benchmark source file (create benchmarks/benchmark.cpp)
set(BENCHMARK_SOURCES 
    benchmark.cpp
    src/Login.cpp
    src/DeribitApi.cpp
)

# Create benchmark executable
add_executable(benchmark-tests ${BENCHMARK_SOURCES})

# Link benchmark libraries
target_link_libraries(benchmark-tests 
    PRIVATE benchmark::benchmark 
    PRIVATE pthread  # Required for Google Benchmark
    PRIVATE Boost::system 
    PRIVATE Boost::coroutine 
    PRIVATE Boost::thread
    PRIVATE OpenSSL::SSL 
    PRIVATE OpenSSL::Crypto
    PRIVATE ${CURL_LIBRARIES}
)