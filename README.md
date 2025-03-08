**# Deribit Trading System**

## Overview  
The **Deribit Trading System** is a high-performance order execution and management system designed for trading on Deribit Testnet. The system is optimized for low-latency operations and real-time market data processing using **Boost.Beast** for WebSocket communication, **OpenSSL** for secure authentication, and **Google Benchmark** for performance evaluation.  

## Features  
- **WebSocket Market Data Streaming** using **Boost.Beast**  
- **Secure Authentication** via **OpenSSL**  
- **Optimized JSON Parsing** with **RapidJSON**  
- **Asynchronous Processing** for non-blocking operations  
- **Performance Benchmarking** using **Google Benchmark**  
- **Efficient Memory Management** with **Move Semantics & Smart Pointers**  

## Dependencies  
The project relies on the following libraries:  

- **C++ Standard**: C++17  
- **Boost Libraries**: `system`, `coroutine`, `thread`  
- **OpenSSL**: Secure authentication  
- **CURL**: HTTP request handling  
- **Google Benchmark**: Performance testing  
- **RapidJSON**: Fast JSON parsing  

### Installing Dependencies  

#### 1. **Boost**  
```sh  
git clone --recursive https://github.com/boostorg/boost.git  

```

#### 2. **OpenSSL** (For Linux)  
```sh  
sudo apt update && sudo apt install libssl-dev  
```

#### 3. **CURL**  
```sh  
sudo apt install libcurl4-openssl-dev  
```

#### 4. **Google Benchmark**  
```sh  
git clone https://github.com/google/benchmark.git  

```

#### 5. **RapidJSON**  
```sh  
git clone https://github.com/Tencent/rapidjson.git  
```

## Flow of Project  
![System Architecture](workflow.png)  

## Build Instructions  
To build the project, use **CMake**:  
```sh  
mkdir build && cd build  
cmake ..  
make  
```

## Executables  
### Main Trading System  
To run the main trading system:  
```sh  
./DeribitTradingSystem  
```

### Benchmark Tests  
To run performance benchmarks:  
```sh  
./benchmark-tests  
```

## Code Structure  
```
DeribitTradingSystem/
├── CMakeLists.txt
├── src/
│   ├── Login.cpp
│   ├── WebSocketClient.cpp
│   ├── DeribitApi.cpp
├── headers/
│   ├── Login.h
│   ├── WebSocketClient.h
│   ├── DeribitApi.h
├── third_party/
│   ├── rapidjson/
│   ├── ...
└── benchmark.cpp
