# cmake_minimum_required(VERSION 3.10)
# project(TradingApp)

# set(CMAKE_CXX_STANDARD 17)

# # Include vcpkg libraries
# find_package(cpr CONFIG REQUIRED)
# find_package(websocketpp CONFIG REQUIRED)

# add_executable(TradingApp main.cpp)  # Replace 'main.cpp' with your source file

# target_link_libraries(TradingApp PRIVATE cpr::cpr websocketpp::websocketpp)


# cmake_minimum_required(VERSION 3.10)
# project(TradingApp)

# set(CMAKE_CXX_STANDARD 17)

# # Include vcpkg libraries
# find_package(cpr CONFIG REQUIRED)
# find_package(websocketpp CONFIG REQUIRED)

# add_executable(TradingApp placeOrder.cpp)  # Updated to point to placeOrder.cpp

# target_link_libraries(TradingApp PRIVATE cpr::cpr websocketpp::websocketpp)


cmake_minimum_required(VERSION 3.10)
project(TradingApp)

set(CMAKE_CXX_STANDARD 17)

# Include vcpkg libraries
find_package(cpr CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)  # Add this line for JSON

add_executable(TradingApp placeOrder.cpp)

# Link the libraries
target_link_libraries(TradingApp PRIVATE cpr::cpr websocketpp::websocketpp nlohmann_json::nlohmann_json)
