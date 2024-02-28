#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

using namespace std;

string escape_string_quotes(string data){
    string res;
    for (char c : data){
    
        if (c == '"'){
            cout << "in if";
            res += "\\\"";
        } else {
            res += c;
        }
    }
    cout << "Escaped string: " << res << std::endl;
    return res;
}

int main() {
    // read file
    // string raw_json_string = '{"GivenName":"Shane", "FamilyName":"Short"}';
    // raw_json_string = escape_string_quotes(raw_json_string);
    string raw_json_string = "{\"GivenName\":\"Shane\", \"FamilyName\":\"Short\"}";
    auto json = nlohmann::json::parse(raw_json_string);

    // mutate the json
    json["value1"] = "new string";

    // write to a stream, or the same file
    std::cout << json; // print the json
}