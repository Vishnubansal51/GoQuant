// #include <iostream>
// #include <string>
// #include <curl/curl.h>
// #include <nlohmann/json.hpp> // JSON library for parsing responses
// bool place_order(const std::string& symbol, double price, int quantity, const std::string& order_type) {
//     CURL* curl;
//     CURLcode res;

//     // Initialize CURL
//     curl = curl_easy_init();
//     if(!curl) {
//         std::cerr << "Failed to initialize CURL" << std::endl;
//         return false;
//     }

//     // Deribit API endpoint for placing orders
//     std::string url = "https://test.deribit.com/api/v2/private/buy";

//     // Construct JSON payload for the order
//     Json::Value order;
//     order["instrument_name"] = symbol;
//     order["amount"] = quantity;
//     order["type"] = order_type;
//     order["price"] = price;

//     Json::StreamWriterBuilder writer;
//     std::string payload = Json::writeString(writer, order);

//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");

//     // Set CURL options
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

//     // Response handler
//     res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Clean up
//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);
//     return true;
// }


// std::string parse_order_response(const std::string& response) {
//     Json::CharReaderBuilder reader;
//     Json::Value jsonData;
//     std::string errs;

//     std::istringstream stream(response);
//     if (!Json::parseFromStream(reader, stream, &jsonData, &errs)) {
//         std::cerr << "Failed to parse JSON: " << errs << std::endl;
//         return "";
//     }

//     return jsonData["result"]["order_id"].asString();
// }


// int main() {
//     std::string symbol = "BTC-PERPETUAL";
//     double price = 50000.0;
//     int quantity = 1;
//     std::string order_type = "limit";

//     if (place_order(symbol, price, quantity, order_type)) {
//         std::cout << "Order placed successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to place order." << std::endl;
//     }

//     return 0;
// }


// WORKING FINE 

// #include <iostream>
// #include <string>
// #include <curl/curl.h>
// #include <nlohmann/json.hpp> // JSON library for parsing responses

// // Define the namespace for convenience
// using json = nlohmann::json;

// bool authenticate(CURL* curl, std::string& auth_token, const std::string& client_id, const std::string& client_secret) {
//     std::string auth_url = "https://test.deribit.com/api/v2/public/auth?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;

//     curl_easy_setopt(curl, CURLOPT_URL, auth_url.c_str());
//     curl_easy_setopt(curl, CURLOPT_POST, 1L);

//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char* ptr, size_t size, size_t nmemb, std::string* data) {
//         data->append(ptr, size * nmemb);
//         return size * nmemb;
//     });
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "Auth failed: " << curl_easy_strerror(res) << std::endl;
//         return false;
//     }

//     // Parse JSON response to extract the access token
//     auto json_response = json::parse(response_data);
//     auth_token = json_response["result"]["access_token"].get<std::string>();
//     return true;
// }


// // bool place_order(const std::string& symbol, double price, int quantity, const std::string& order_type) {
// //     CURL* curl;
// //     CURLcode res;

// //     // Initialize CURL
// //     curl = curl_easy_init();
// //     if(!curl) {
// //         std::cerr << "Failed to initialize CURL" << std::endl;
// //         return false;
// //     }

// //     // Deribit API endpoint for placing orders
// //     std::string url = "https://test.deribit.com/api/v2/private/buy";

// //     // Construct JSON payload for the order using nlohmann::json
// //     json order;
// //     order["instrument_name"] = symbol;
// //     order["amount"] = quantity;
// //     order["type"] = order_type;
// //     order["price"] = price;

// //     std::string payload = order.dump(); // Serialize JSON object to string

// //     struct curl_slist* headers = nullptr;
// //     headers = curl_slist_append(headers, "Content-Type: application/json");

// //     // Set CURL options
// //     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
// //     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
// //     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

// //     // Response handler
// //     res = curl_easy_perform(curl);
// //     if (res != CURLE_OK) {
// //         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
// //         curl_easy_cleanup(curl);
// //         return false;
// //     }

// //     // Clean up
// //     curl_slist_free_all(headers);
// //     curl_easy_cleanup(curl);
// //     return true;
// // }


// bool place_order(const std::string& symbol, double price, int quantity, const std::string& order_type,
//                  const std::string& client_id, const std::string& client_secret) {
//     CURL* curl;
//     CURLcode res;

//     // Initialize CURL
//     curl = curl_easy_init();
//     if (!curl) {
//         std::cerr << "Failed to initialize CURL" << std::endl;
//         return false;
//     }

//     // Authenticate and obtain access token
//     std::string auth_token;
//     if (!authenticate(curl, auth_token, client_id, client_secret)) {
//         std::cerr << "Authentication failed!" << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Deribit API endpoint for placing orders
//     std::string url = "https://test.deribit.com/api/v2/private/buy";

//     // Construct JSON payload for the order
//     json order;
//     order["instrument_name"] = symbol;
//     order["amount"] = quantity;
//     order["type"] = order_type;
//     order["price"] = price;

//     std::string payload = order.dump();

//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

//     // Set CURL options
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

//     // Response handler
//     res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Clean up
//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);
//     return true;
// }

// std::string parse_order_response(const std::string& response) {
//     // Parse JSON response using nlohmann::json
//     json jsonData;
//     try {
//         jsonData = json::parse(response);
//     } catch (json::parse_error& e) {
//         std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
//         return "";
//     }

//     // Return the order_id from the response JSON if present
//     return jsonData["result"]["order_id"].get<std::string>();
// }

// int main() {
//     std::string symbol = "BTC-PERPETUAL";
//     double price = 50000.0;
//     int quantity = 1;
//     std::string order_type = "limit";

//     // Replace these with your actual client ID and client secret
//     std::string client_id = "4JtGxFSE ";
//     std::string client_secret = "J8NfnMFTAPjBiJ-7nAYUgjszhMqcHoruKMFBR_wBTWo";

//     if (place_order(symbol, price, quantity, order_type, client_id, client_secret)) {
//         std::cout << "Order placed successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to place order." << std::endl;
//     }

//     return 0;
// }


// Authenticate function to obtain access token
// bool authenticate(CURL* curl, std::string& auth_token, const std::string& client_id, const std::string& client_secret) {

 
//     std::string auth_url = "https://test.deribit.com/api/v2/public/auth?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;

//     std::cout << "Auth URL: " << auth_url << std::endl;



//     curl_easy_setopt(curl, CURLOPT_URL, auth_url.c_str());
//     curl_easy_setopt(curl, CURLOPT_POST, 1L);

//     curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

//     curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    
//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "Auth failed: " << curl_easy_strerror(res) << std::endl;
//         return false;
//     }
//   std::cout << "Response Data: " << response_data << std::endl;
//     // Parse JSON response to extract the access token
//     // auto json_response = json::parse(response_data);
//     // auth_token = json_response["result"]["access_token"].get<std::string>();
//     // return true;



//     try {
//         auto json_response = json::parse(response_data);
//         if (json_response.contains("result")) {
//             auth_token = json_response["result"]["access_token"].get<std::string>();
//             return true;
//         } else {
//             std::cerr << "Authentication error: " << json_response["error"]["message"].get<std::string>() << std::endl;
//             return false;
//         }
//     } catch (json::parse_error& e) {
//         std::cerr << "JSON Parse Error: " << e.what() << std::endl;
//         return false;
//     }
// }



// bool authenticate(CURL* curl, std::string& auth_token, const std::string& client_id, const std::string& client_secret) {
//     std::string auth_url = "https://test.deribit.com/api/v2/public/auth?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;

//     std::cout << "Auth URL: " << auth_url << std::endl;

//     curl_easy_setopt(curl, CURLOPT_URL, auth_url.c_str());
//     curl_easy_setopt(curl, CURLOPT_POST, 1L);
    
//     // Enable verbose output
//     curl_easy_setopt(curl, CURLOPT_VERBOSE, 2L); // Increased verbosity
//     curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Set timeout

//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

//     // Set headers
//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, "Expect:"); // Remove Expect header
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
//     // Optionally disable SSL verification (for debugging only)
//     // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

// CURLcode res = curl_easy_perform(curl);
// if (res != CURLE_OK) {
//     std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
// } else {
//     std::cout << "Request sent successfully!" << std::endl;
//     std::cout << "Response Data: " << response_data << std::endl;
// }

//     std::cout << " *************Response Data: " << response_data << std::endl;

//     // Parse JSON response
//     try {
//         auto json_response = json::parse(response_data);
//         if (json_response.contains("result")) {
//             auth_token = json_response["result"]["access_token"].get<std::string>();
//             return true;
//         } else {
//             std::cerr << "Authentication error: " << json_response["error"]["message"].get<std::string>() << std::endl;
//             return false;
//         }
//     } catch (json::parse_error& e) {
//         std::cerr << "JSON Parse Error: " << e.what() << std::endl;
//         return false;
//     }
// }



// Function to place an order
// bool place_order(const std::string& symbol, double price, int quantity, const std::string& order_type,
//                  const std::string& client_id, const std::string& client_secret) {
//     CURL* curl;
//     CURLcode res;

//     // Initialize CURL
//     curl = curl_easy_init();
    
//     if (!curl) {
//         std::cerr << "Failed to initialize CURL" << std::endl;
//         return false;
//     }

//     // Authenticate and obtain access token
//     std::string auth_token;
   

//     if (!authenticate(curl, auth_token, client_id, client_secret)) {
//         std::cerr << "Authentication failed!" << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }
//     else{
//         std::cerr << "true!" << std::endl;
     
//     }


//     // Deribit API endpoint for placing orders
//     std::string url = "https://test.deribit.com/api/v2/private/buy";

//     // Construct JSON payload for the order
//     json order;
//     order["instrument_name"] = symbol;
//     order["amount"] = quantity;
//     order["type"] = order_type;
//     order["price"] = price;

//     std::string payload = order.dump();

//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

//     // Set CURL options
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    
//     // Response handler
//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

//     res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Clean up
//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);

//     // Parse the response
//     auto json_response = json::parse(response_data);
//     if (json_response.contains("result")) {
//         std::cout << "Order placed successfully. Order ID: " << json_response["result"]["order_id"].get<std::string>() << std::endl;
//         return true;
//     } else {
//         std::cerr << "Failed to place order: " << json_response["error"]["message"].get<std::string>() << std::endl;
//         return false;
//     }
// }


// bool place_order(const std::string& symbol, int quantity, const std::string& order_type, const std::string& client_id, const std::string& client_secret) {
//     CURL* curl;
//     CURLcode res;

//     curl = curl_easy_init();
//     if (!curl) {
//         std::cerr << "Failed to initialize CURL" << std::endl;
//         return false;
//     }

//     std::string auth_token;
//     if (!authenticate(curl, auth_token, client_id, client_secret)) {
//         std::cerr << "Authentication failed!" << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Deribit API endpoint for placing orders
//     std::string url = "https://test.deribit.com/api/v2/private/buy?amount=" + std::to_string(quantity) +
//                       "&instrument_name=" + symbol + "&type=" + order_type + "&label=market0000234";

//     std::cout << "Order URL: " << url << std::endl; // Debugging line to print the full URL

//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

//     res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
//         curl_slist_free_all(headers);
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);

//     try {
//         auto json_response = json::parse(response_data);
//         if (json_response.contains("result")) {
//             std::cout << "Order placed successfully. Order ID: " << json_response["result"]["order_id"].get<std::string>() << std::endl;
//             return true;
//         } else {
//             std::cerr << "Failed to place order: " << json_response["error"]["message"].get<std::string>() << std::endl;
//             return false;
//         }
//     } catch (json::parse_error& e) {
//         std::cerr << "JSON Parse Error: " << e.what() << std::endl;
//         return false;
//     }
// }

// bool place_order(const std::string& symbol, int quantity, const std::string& order_type, const std::string& client_id, const std::string& client_secret) {
//     CURL* curl;
//     CURLcode res;

//     curl = curl_easy_init();
//     if (!curl) {
//         std::cerr << "Failed to initialize CURL" << std::endl;
//         return false;
//     }

//     std::string auth_token;
//     if (!authenticate(curl, auth_token, client_id, client_secret)) {
//         std::cerr << "Authentication failed!" << std::endl;
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     // Construct the URL according to the provided curl example
//     std::string url = "https://test.deribit.com/api/v2/private/buy?amount=" + std::to_string(quantity) +
//                       "&instrument_name=" + symbol + "&label=market0000234&type=" + order_type;

//     struct curl_slist* headers = nullptr;
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

//     std::string response_data;
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

//     res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
//         curl_slist_free_all(headers);
//         curl_easy_cleanup(curl);
//         return false;
//     }

//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);

//     try {
//         auto json_response = json::parse(response_data);
//         if (json_response.contains("result")) {
//             std::cout << "Order placed successfully. Order ID: " << json_response["result"]["order_id"].get<std::string>() << std::endl;
//             return true;
//         } else {
//             std::cerr << "Failed to place order: " << json_response["error"]["message"].get<std::string>() << std::endl;
//             return false;
//         }
//     } catch (json::parse_error& e) {
//         std::cerr << "JSON Parse Error: " << e.what() << std::endl;
//         return false;
//     }
// }






// int main() {
//     std::cout << "Placing order..." << std::endl;
    
//     std::string symbol = "BTC-PERPETUAL";
//     double price = 67017;
//     int quantity = 1;
//     std::string order_type = "limit";
    

//     // Replace these with your actual client ID and client secret
//     std::string client_id = "4JtGxFSE";  // Update with your client ID
//     std::string client_secret = "J8NfnMFTAPjBiJ-7nAYUgjszhMqcHoruKMFBR_wBTWo";  // Update with your client secret

    

//     if (place_order(symbol, price, quantity, order_type, client_id, client_secret)) {
//         std::cout << "Order placed successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to place order." << std::endl;
//     }

//     return 0;
// }


/ int main() {
//     std::cout << "Placing order..." << std::endl;

//     // std::string symbol = "ETH-PERPETUAL";  // Use the instrument name as per your needs
//     // int quantity = 40;                       // Update according to your order amount
//     // std::string order_type = "market";   

//     // Replace these with your actual client ID and client secret
//     std::string client_id = "4JtGxFSE";  // Update with your client ID 
//     std::string client_secret = "J8NfnMFTAPjBiJ-7nAYUgjszhMqcHoruKMFBR_wBTWo";  // Update with your client secret 

//     // **********BUYING************* 
// //  if (place_order(symbol, quantity, order_type, client_id, client_secret)) {
// //         std::cout << "Order placed successfully!" << std::endl;
// //     } else {
// //         std::cerr << "Failed to place order." << std::endl;
// //     }
//     //  FOR ***********8SELLING****************
//     // if (place_sell_order(symbol, quantity, order_type, client_id, client_secret)) {
//     //     std::cout << "Sell order placed successfully!" << std::endl;
//     // } else {
//     //     std::cerr << "Failed to place sell order." << std::endl;
//     // }



//     std::string symbol = "ETH-PERPETUAL";  // Use the instrument name as per your needs
//     int depth = 5;                          // Update the depth according to your requirements

//     if (get_order_book(symbol, depth)) {
//         std::cout << "Order book fetched successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to fetch order book." << std::endl;
//     }



//     // TO GET POSITIONS 

//     //  std::string currency = "BTC"; // Specify the currency, e.g., "BTC" or "ETH"
//     // std::string kind = "";        // Optional: specify "future", "option", or leave empty for all types

//     // if (get_positions(client_id, client_secret, currency, kind)) {
//     //     std::cout << "Positions retrieved successfully!" << std::endl;
//     // } else {
//     //     std::cerr << "Failed to retrieve positions." << std::endl;
//     // }


//     // TO MODIFY THE ORDER 

//     std::string order_id = "438994";                 // Replace with the ID of the order to modify
//     int amount = 4;                                  // New amount
//     double price = 222;                              // New price

//     if (modify_order(client_id, client_secret, order_id, amount, price)) {
//         std::cout << "Order modified successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to modify order." << std::endl;
//     }

//     //********* TO CANCEL THE ORDER
//      std::string order_id = "ETH-SLIS-12";            // Replace with the ID of the order to cancel

//     if (cancel_order(client_id, client_secret, order_id)) {
//         std::cout << "Order canceled successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to cancel order." << std::endl;
//     }


//     return 0;
// }






// int main() {
//     std::string client_id = "ezSjeAG1";  // Replace with your actual client ID
//     std::string client_secret = "kchIub_gfVs4PJ-8Vt3oCJxZOpdZ9Tv8GRVmQWIn75Q";  // Replace with your actual client secret

//     int choice;
//     bool exit = false;

//     while (!exit) {
//         display_menu();
//         std::cin >> choice;

//         switch (choice) {
//             case 1: {  // Buy Order
//                 std::string symbol;
//                 int quantity;
//                 std::string order_type;

//                 std::cout << "Enter symbol (e.g., ETH-PERPETUAL): ";
//                 std::cin >> symbol;
//                 std::cout << "Enter quantity: ";
//                 std::cin >> quantity;
//                 std::cout << "Enter order type (e.g., market): ";
//                 std::cin >> order_type;

//                 if (place_order(symbol, quantity, order_type, client_id, client_secret)) {
//                     std::cout << "Buy order placed successfully!\n";
//                 } else {
//                     std::cerr << "Failed to place buy order.\n";
//                 }
//                 break;
//             }
//             case 2: {  // Sell Order
//                 std::string symbol;
//                 int quantity;
//                 std::string order_type;

//                 std::cout << "Enter symbol (e.g., ETH-PERPETUAL): ";
//                 std::cin >> symbol;
//                 std::cout << "Enter quantity: ";
//                 std::cin >> quantity;
//                 std::cout << "Enter order type (e.g., market): ";
//                 std::cin >> order_type;

//                 if (place_sell_order(symbol, quantity, order_type, client_id, client_secret)) {
//                     std::cout << "Sell order placed successfully!\n";
//                 } else {
//                     std::cerr << "Failed to place sell order.\n";
//                 }
//                 break;
//             }
//             case 3: {  // Get Order Book
//                 std::string symbol;
//                 int depth;

//                 std::cout << "Enter symbol (e.g., ETH-PERPETUAL): ";
//                 std::cin >> symbol;
//                 std::cout << "Enter depth: ";
//                 std::cin >> depth;

//                 if (get_order_book(symbol, depth)) {
//                     std::cout << "Order book fetched successfully!\n";
//                 } else {
//                     std::cerr << "Failed to fetch order book.\n";
//                 }
//                 break;
//             }
//             case 4: {  // View Positions
//                 std::string currency;
//                 std::string kind;

//                 std::cout << "Enter currency (e.g., BTC, ETH): ";
//                 std::cin >> currency;
//                 std::cout << "Enter kind (future, option, or leave empty for all): ";
//                 std::cin.ignore(); // to clear the newline from the input buffer
//                 std::getline(std::cin, kind); // allow empty input for kind

//                 if (get_positions(client_id, client_secret, currency, kind)) {
//                     std::cout << "Positions retrieved successfully!\n";
//                 } else {
//                     std::cerr << "Failed to retrieve positions.\n";
//                 }
//                 break;
//             }
//             case 5: {  // Modify Order
//                 std::string order_id;
//                 int amount;
//                 double price;

//                 std::cout << "Enter order ID: ";
//                 std::cin >> order_id;
//                 std::cout << "Enter new amount: ";
//                 std::cin >> amount;
//                 std::cout << "Enter new price: ";
//                 std::cin >> price;

//                 if (modify_order(client_id, client_secret, order_id, amount, price)) {
//                     std::cout << "Order modified successfully!\n";
//                 } else {
//                     std::cerr << "Failed to modify order.\n";
//                 }
//                 break;
//             }
//             case 6: {  // Cancel Order
//                 std::string order_id;

//                 std::cout << "Enter order ID: ";
//                 std::cin >> order_id;

//                 if (cancel_order(client_id, client_secret, order_id)) {
//                     std::cout << "Order canceled successfully!\n";
//                 } else {
//                     std::cerr << "Failed to cancel order.\n";
//                 }
//                 break;
//             }
//             case 0:
//                 std::cout << "Exiting program.\n";
//                 exit = true;
//                 break;
//             default:
//                 std::cerr << "Invalid choice. Please try again.\n";
//         }
//     }

//     return 0;
// }



