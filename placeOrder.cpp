
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp> // JSON library for parsing responses

// define the namespace for convenience
using json = nlohmann::json;

// Function to handle the response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

bool authenticate(CURL *curl, std::string &auth_token, const std::string &client_id, const std::string &client_secret)
{
    std::string auth_url = "https://test.deribit.com/api/v2/public/auth?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;

    std::cout << "Auth URL: " << auth_url << std::endl;

    // Use GET request by not setting POST
    curl_easy_setopt(curl, CURLOPT_URL, auth_url.c_str());

    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Set headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Expect:"); // Remove Expect header
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        std::cout << "Response received successfully!" << std::endl;
    }

    // Parse JSON response
    try
    {
        auto json_response = json::parse(response_data);
        if (json_response.contains("result"))
        {
            auth_token = json_response["result"]["access_token"].get<std::string>();
            return true;
        }
        else
        {
            std::cerr << "Authentication error: " << json_response["error"]["message"].get<std::string>() << std::endl;
            return false;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return false;
    }
}

json place_order(const std::string &symbol, int quantity, const std::string &order_type, const std::string &client_id, const std::string &client_secret, double price = 0, double stop_price = 0)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "CURL initialization failed"}};
    }

    std::string auth_token;
    if (!authenticate(curl, auth_token, client_id, client_secret))
    {
        std::cerr << "Authentication failed!" << std::endl;
        curl_easy_cleanup(curl);
        return json{{"error", "Authentication failed"}};
    }

    std::string url = "https://test.deribit.com/api/v2/private/";

    // Determine the appropriate endpoint based on order type
    if (order_type == "market" || order_type == "market_limit")
    {
        url += "buy?amount=" + std::to_string(quantity) + "&instrument_name=" + symbol + "&type=" + order_type;
    }
    else
    {
        url += "buy?amount=" + std::to_string(quantity) + "&instrument_name=" + symbol + "&type=" + order_type;

        // Add price if required by the order type
        if (order_type == "limit" || order_type == "stop_limit" || order_type == "take_limit" || order_type == "market_limit")
        {
            url += "&price=" + std::to_string(price);
        }

        // Add stop price if required
        if (order_type == "stop_limit" || order_type == "stop_market" || order_type == "trailing_stop")
        {
            url += "&stop_price=" + std::to_string(stop_price);
        }
    }

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return json{{"error", curl_easy_strerror(res)}};
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // try
    // {

    //     auto json_response = json::parse(response_data);

    //     // Check if the "result" key exists and contains order and trade details
    //     if (json_response.contains("result"))
    //     {
    //         // std::cout << "Response Data: " << response_data << std::endl;
    //         std::cout << "Order ID: " << json_response["result"]["order"]["order_id"] << std::endl;

    //         std::cout << "placed" << std::endl;

    //         return json_response["result"]; // Return the result part of the JSON response
    //     }
    //     else
    //     {
    //         return json{{"error", "Failed to place order"}};
    //     }
    // }
    // catch (json::parse_error &e)
    // {
    //     std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    //     return json{{"error", "JSON parsing error"}};
    // }
    // catch (std::exception &e)
    // {
    //     std::cerr << "Exception: " << e.what() << std::endl;
    //     return json{{"error", "An exception occurred"}};
    // }
    try
    {
        auto json_response = json::parse(response_data);

        // Check if the response contains the "result" key with order details
        if (json_response.contains("result") && json_response["result"].contains("order"))
        {
            std::cout << "Order placed successfully. Order ID: "
                      << json_response["result"]["order"]["order_id"].get<std::string>() << std::endl;
            return json_response["result"];
        }
        else
        {
            json error_response = {{"error", "Failed to place order"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return json{{"error", "Exception: " + std::string(e.what())}};
    }
}

json get_positions(const std::string &client_id, const std::string &client_secret, const std::string &currency, const std::string &kind = "")
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "Failed to initialize CURL"}};
    }

    std::string auth_token;
    if (!authenticate(curl, auth_token, client_id, client_secret))
    {
        std::cerr << "Authentication failed!" << std::endl;
        curl_easy_cleanup(curl);
        return json{{"error", "Authentication failed!"}};
    }

    std::string url = "https://test.deribit.com/api/v2/private/get_positions?currency=" + currency;
    if (!kind.empty())
    {
        url += "&kind=" + kind;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return json{{"error", "CURL error: " + std::string(curl_easy_strerror(res))}};
    }

    try
    {
        auto json_response = json::parse(response_data);

        // Check if the "result" key exists and return it
        if (json_response.contains("result"))
        {
            return json_response; // Return the entire JSON response
        }
        else
        {
            json error_response = {{"error", "Failed to retrieve positions"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
}

json place_sell_order(const std::string &symbol, int quantity, const std::string &order_type, const std::string &client_id, const std::string &client_secret, double price = 0.0, double stop_price = 0.0)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "Failed to initialize CURL"}};
    }

    std::string auth_token;
    if (!authenticate(curl, auth_token, client_id, client_secret))
    {
        std::cerr << "Authentication failed!" << std::endl;
        curl_easy_cleanup(curl);
        return json{{"error", "Authentication failed!"}};
    }

    // Construct the URL for selling order
    std::string url = "https://test.deribit.com/api/v2/private/sell?amount=" + std::to_string(quantity) +
                      "&instrument_name=" + symbol + "&label=sellOrder001&type=" + order_type;

    // Add price parameter if applicable
    if (order_type == "limit" || order_type == "stop_limit" || order_type == "take_limit" || order_type == "market_limit")
    {
        url += "&price=" + std::to_string(price);
    }

    // Add stop price if applicable
    if (order_type == "stop_limit" || order_type == "stop_market" || order_type == "trailing_stop")
    {
        url += "&stop_price=" + std::to_string(stop_price);
    }

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return json{{"error", "CURL error: " + std::string(curl_easy_strerror(res))}};
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try
    {
        auto json_response = json::parse(response_data);

        // Check if the "result" key exists and access the order details
        if (json_response.contains("result") && json_response["result"].contains("order"))
        {
        
            return json_response["result"]; // Return the entire JSON response as it contains order details
        }
        else
        {
            // Handle error if order placement fails
            json error_response = {{"error", "Failed to place sell order"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return json{{"error", "Exception: " + std::string(e.what())}};
    }
}

json get_order_book(const std::string &symbol, int depth)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "Failed to initialize CURL"}};
    }

    // Construct the order book URL
    std::string url = "https://test.deribit.com/api/v2/public/get_order_book?depth=" + std::to_string(depth) +
                      "&instrument_name=" + symbol;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Set headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return json{{"error", "CURL error: " + std::string(curl_easy_strerror(res))}};
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try
    {
        auto json_response = json::parse(response_data);

        // Check if the "result" key exists and access the order book details
        if (json_response.contains("result"))
        {
            return json_response; // Return the entire JSON response
        }
        else
        {
            json error_response = {{"error", "Failed to get order book"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return json{{"error", "Exception: " + std::string(e.what())}};
    }
}

json modify_order(const std::string &client_id, const std::string &client_secret, const std::string &order_id, int amount, double price, const std::string &advanced = "")
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "Failed to initialize CURL"}};
    }

    std::string auth_token;
    if (!authenticate(curl, auth_token, client_id, client_secret))
    {
        std::cerr << "Authentication failed!" << std::endl;
        curl_easy_cleanup(curl);
        return json{{"error", "Authentication failed"}};
    }

    std::cout << "Auth Token: " << auth_token << std::endl;

    // Construct the URL for modifying the order
    std::string url = "https://test.deribit.com/api/v2/private/edit?amount=" + std::to_string(amount) +
                      "&order_id=" + order_id + "&price=" + std::to_string(price);

    // Include `advanced` parameter only if it’s non-empty
    if (!advanced.empty())
    {
        url += "&advanced=" + advanced;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Set headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return json{{"error", "CURL error: " + std::string(curl_easy_strerror(res))}};
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try
    {
        auto json_response = json::parse(response_data);

        // Check if the "result" key exists and access the modified order details
        if (json_response.contains("result"))
        {
            return json_response; // Return the entire JSON response
        }
        else
        {
            json error_response = {{"error", "Failed to modify order"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return json{{"error", "Exception: " + std::string(e.what())}};
    }
}

json cancel_order(const std::string &client_id, const std::string &client_secret, const std::string &order_id)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return json{{"error", "Failed to initialize CURL"}};
    }

    std::string auth_token;
    if (!authenticate(curl, auth_token, client_id, client_secret))
    {
        std::cerr << "Authentication failed!" << std::endl;
        curl_easy_cleanup(curl);
        return json{{"error", "Authentication failed"}};
    }

    std::cout << "Auth Token: " << auth_token << std::endl;

    // Construct the URL for canceling the order
    std::string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Set headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return json{{"error", "CURL error: " + std::string(curl_easy_strerror(res))}};
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try
    {
        auto json_response = json::parse(response_data);
      
        // Check if the "result" key exists and access the cancel order details
        if (json_response.contains("result"))
        {
            return json_response; // Return the entire JSON response
        }
        else
        {
            json error_response = {{"error", "Failed to cancel order"}};
            if (json_response.contains("error") && json_response["error"].contains("message"))
            {
                error_response["message"] = json_response["error"]["message"].get<std::string>();
            }
            return error_response;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{{"error", "JSON Parse Error: " + std::string(e.what())}};
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return json{{"error", "Exception: " + std::string(e.what())}};
    }
}


int main()
{


    std::string client_id = "ezSjeAG1"; // Replace with actual client ID
    std::string client_secret = "kchIub_gfVs4PJ-8Vt3oCJxZOpdZ9Tv8GRVmQWIn75Q"; // Replace with actual client secret

    while (true)
    {
        std::cout << "\nWelcome to Deribit Trading System\n";
        std::cout << "Please select an action:\n";
        std::cout << "1. Buy Order\n";
        std::cout << "2. Sell Order\n";
        std::cout << "3. Cancel Order\n";
        std::cout << "4. Modify Order\n";
        std::cout << "5. Get Order Book\n";
        std::cout << "6. View Open Positions\n";
        std::cout << "7. Exit\n";
        
        int action;
        std::cin >> action;

        if (action == 1 || action == 2)
        { 
            // Buy or Sell Order
            std::string symbol;
            int quantity;
            std::string order_type;
            double price = 0.0;
            double stop_price = 0.0;

            std::cout << "Enter the instrument symbol (e.g., ETH-PERPETUAL): ";
            std::cin >> symbol;
            std::cout << "Enter the quantity: ";
            std::cin >> quantity;
            std::cout << "Enter order type (market, limit, stop_limit, etc.): ";
            std::cin >> order_type;

            // Prompt for additional parameters based on the order type
            if (order_type == "limit" || order_type == "stop_limit" || order_type == "take_limit" || order_type == "market_limit")
            {
                std::cout << "Enter the price: ";
                std::cin >> price;
            }
            if (order_type == "stop_limit" || order_type == "stop_market" || order_type == "trailing_stop")
            {
                std::cout << "Enter the stop price: ";
                std::cin >> stop_price;
            }

            json order_response;
            if (action == 1)
            {
                order_response = place_order(symbol, quantity, order_type, client_id, client_secret, price, stop_price);
            }
            else
            {
                order_response = place_sell_order(symbol, quantity, order_type, client_id, client_secret, price, stop_price);
            }

            if (!order_response.is_null() && order_response.contains("order"))
            {
                std::cout << "Order placed successfully! Response:\n" << order_response.dump(4) << std::endl;
            }
            else if (order_response.contains("error"))
            {
                std::cerr << "Error: " << order_response["error"] << std::endl;
                if (order_response.contains("message"))
                {
                    std::cerr << "Message: " << order_response["message"] << std::endl;
                }
            }
            else
            {
                std::cerr << "Failed to place " << (action == 1 ? "buy" : "sell") << " order." << std::endl;
            }
        }
        else if (action == 3)
        { 
            // Cancel Order
            std::string order_id;
            std::cout << "Enter the order ID to cancel: ";
            std::cin >> order_id;

            json cancel_response = cancel_order(client_id, client_secret, order_id);

            if (cancel_response.contains("error"))
            {
                std::cerr << "Failed to cancel order: " << cancel_response["error"].get<std::string>() << std::endl;
                if (cancel_response.contains("message"))
                {
                    std::cerr << "Error message: " << cancel_response["message"].get<std::string>() << std::endl;
                }
            }
            else
            {
                std::cout << "Order canceled successfully!" << std::endl;
                std::cout << "Cancel Order Response: " << cancel_response.dump(4) << std::endl;
            }
        }
        else if (action == 4) 
        {
            // Modify Order
            std::string order_id;
            int amount;
            double price;

            std::cout << "Enter the order ID to modify: ";
            std::cin >> order_id;
            std::cout << "Enter the new amount: ";
            std::cin >> amount;
            std::cout << "Enter the new price: ";
            std::cin >> price;

            json response = modify_order(client_id, client_secret, order_id, amount, price);

            if (response.contains("error"))
            {
                std::cerr << "Failed to modify order: " << response["error"].get<std::string>() << std::endl;
            }
            else
            {
                std::cout << "Order modified successfully with response: " << response.dump(4) << std::endl;
            }
        }



        else if (action == 5)
        { 
            // Get Order Book
            std::string symbol;
            int depth;

            std::cout << "Enter the instrument symbol: ";
            std::cin >> symbol;
            std::cout << "Enter the depth of the order book: ";
            std::cin >> depth;
            json order_book_response = get_order_book(symbol, depth);

            if (order_book_response.contains("error"))
            {
                std::cerr << "Failed to fetch order book: " << order_book_response["error"].get<std::string>() << std::endl;
            }
            else
            {
                std::cout << "Order Book Response: " << order_book_response.dump(4) << std::endl;
                std::cout << "Order book fetched successfully!" << std::endl;
            }
        }
        else if (action == 6)
        { 
            // View Open Positions
            std::string currency, kind;

            std::cout << "Enter the currency (e.g., BTC, ETH): ";
            std::cin >> currency;
            std::cout << "Enter the kind (future, option, or leave empty for all): ";
            std::cin.ignore();
            std::getline(std::cin, kind);

            json positions_response = get_positions(client_id, client_secret, currency, kind);

            if (positions_response.contains("error"))
            {
                std::cerr << "Failed to retrieve positions: " << positions_response["error"].get<std::string>() << std::endl;
            }
            else
            {
                std::cout << "Positions Response: " << positions_response.dump(4) << std::endl;
                std::cout << "Positions retrieved successfully!" << std::endl;
            }
        }
        else if (action == 7)
        {
            // Exit the program
            std::cout << "Exiting the Deribit Trading System. Goodbye!" << std::endl;
            break;
        }
        else
        {
            std::cerr << "Invalid action selected. Please try again." << std::endl;
        }
    }

    return 0;
}

