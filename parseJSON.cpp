#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    // read file
    auto json = nlohmann::json::parse("{\"GivenName\":\"Shane\", \"FamilyName\":\"Short\"}");//"{\"value1\": \"string\"}");

    // mutate the json
    json["value1"] = "new string";

    // write to a stream, or the same file
    std::cout << json; // print the json
}