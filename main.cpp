/*
 refactoring
 */


#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>

enum class LineType {
    KEY_VALUE,
    EMPTY,
    UNKNOWN
};

enum class ValueType {
    BOOLEAN,
    STRING,
    NUMBER
};

struct ParsedLine {
    LineType type;
    std::string key;
    std::string value;
};

struct ConfigValue {
    ValueType type;
    std::string raw;
};

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(' ');
    size_t end = str.find_last_not_of(' ');

    if (start == std::string::npos) return "";

    return str.substr(start, end - start + 1);
}

bool is_json(std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;



    while (std::getline(file, line)) {
        if (line[0] != '{' and line[0] != '[') return false;
        else return true;
    }
}

std::vector<std::string> load_file(std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return {};
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

LineType classify_line(const std::string& line) {
    std::string trimmed_line = trim(line);

    if (trimmed_line.empty()) return LineType::EMPTY;
    else if (trimmed_line.find_first_of(':') != std::string::npos) return LineType::KEY_VALUE;
    else return LineType::UNKNOWN;
}

std::string to_string(LineType type) {
    switch (type) {
            case LineType::KEY_VALUE: return "KEY_VALUE";
            case LineType::EMPTY: return "EMPTY";
            case LineType::UNKNOWN: return "UNKNOWN";
    }
}

auto delete_quotes(const std::string &line) {

    std::pair<std::string, std::string> result;

    std::string trimmed_line = trim(line);

    size_t pos = trimmed_line.find_first_of(':');

    if (pos == std::string::npos) {
        std::cerr << "Error, pos is npos";
        return result;
    }

    std::string key = trimmed_line.substr(0, pos);
    std::string value = trimmed_line.substr(pos+1, trimmed_line.length() - pos - 2);

    key = trim(key);
    value = trim(value);

    if (key.find_first_of('"') != std::string::npos) {
        key = key.substr(key.find_first_of('"') + 1, key.find_last_of('"') - key.find_first_of('"') - 1);
    }

    if (value.find_first_of('"') != std::string::npos) {
        value = value.substr(value.find_first_of('"') + 1, value.find_last_of('"') - value.find_first_of('"') - 1);
    }

    result.first = key;
    result.second = value;

    return result;
    /*
    find_first and find_last can return minus statament and it give UB!
    */

}

ParsedLine parse_line(const std::string& line) {
    ParsedLine parsed_line;
    LineType type = classify_line(line);
    // std::string trimmed_line = trim(line);

    parsed_line.type = type;



    if (parsed_line.type == LineType::KEY_VALUE) {

        std::string key = delete_quotes(line).first;
        std::string value = delete_quotes(line).second;

        parsed_line.key = key;
        parsed_line.value = value;

    }

    return parsed_line;
}

ConfigValue detect_type(const std::string& value) {
    ConfigValue parsed_line;
    parsed_line.raw = trim(value);

    if (parsed_line.raw.empty()) {
        parsed_line.type = ValueType::STRING;
        return parsed_line;
    }

    size_t isSym = (parsed_line.raw[0] == '+' || parsed_line.raw[0] == '-') ? 1 : 0;

    if (parsed_line.raw == "true" || parsed_line.raw == "false") {
        parsed_line.type = ValueType::BOOLEAN;
    }
    else if (isSym < parsed_line.raw.size() && std::all_of(parsed_line.raw.begin() + isSym, parsed_line.raw.end(), ::isdigit)) {
        parsed_line.type = ValueType::NUMBER;
    }
    else {
        parsed_line.type = ValueType::STRING;
    }

    return parsed_line;
}

std::map<std::string, ConfigValue>parse(const std::vector<std::string> &lines) {
    std::map<std::string, ConfigValue> result;

    for (const auto& line : lines) {
        ParsedLine parsed_line = parse_line(line);
        if (parsed_line.type == LineType::KEY_VALUE) {
            result[parsed_line.key] = detect_type(parsed_line.value);
        }
    }

    return result;
}

ConfigValue get_value(std::map<std::string, ConfigValue> &config, const std::string& key) {
    //std::string result;

    if (config.find(key) == config.end()) {
        std::cerr << "Error, no value found for key: " << key << std::endl;
        return {ValueType::STRING, ""};
    }

    //result = config[key];

    return config[key];
}

void set_value(std::map<std::string, ConfigValue> &config, const std::string& key, const std::string& value) {
    std::string result;

    if (config.find(key) == config.end()) {
        std::cerr << "Error, key not found: " << key << std::endl;
        return;
    }

    //result = config[key] = value;

    // result = config[key];

    config[key] = detect_type(value);

}

void save_to_file(std::map<std::string, ConfigValue> &config, const std::string& filename) {
    std::ofstream file;

    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "{" << std::endl;

    int i = 1;
    for (const auto& [key, value] : config) {
        file << "  \"" << key << "\": ";

        if (value.type == ValueType::STRING) file << "\"" << value.raw << "\"";
        else file << value.raw;

        if (i < config.size()) file << ",";
        file << std::endl;
        i++;
    }

    file << "}" << std::endl;

    file.close();
}

/*
 DOesnt work with value '{' and '['
 bugs
*/

int main() {

    std::cout << "Welcome to json-parser!" << std::endl;
    std::cout << "Please, enter path to file" << std::endl;
    std::string filename;
    getline(std::cin, filename);
    std::map<std::string, ConfigValue> config = parse(load_file(filename));
    std::cout << "File loaded and parsed" << std::endl;

    while (is_json(filename)) {
        std::cout << "Please, enter:\n1 - Get value \n2 - Set value\n3 - Save to file\nq - to exit" << std::endl;
        std::string user_input;
        getline(std::cin, user_input);

        switch (user_input[0]) {
            case '1': {
                std::cout << "Enter the key: " << std::endl;
                std::string key;
                getline(std::cin, key);

                ConfigValue value = get_value(config, key);

                std::cout << "Value: " << value.raw << std::endl;
                break;
            }
            case '2': {
                std::string key, value;
                std::cout << "Enter the key: " << std::endl;
                getline(std::cin, key);
                std::cout << "Enter the value: " << std::endl;
                getline(std::cin, value);

                set_value(config, key, value);

                std::cout << "Value is update: " << get_value(config, key).raw << " = " << value << std::endl;
                break;
            }
            case '3': {
                std::string user_filename;
                std::cout << "Enter the filename: " << std::endl;
                getline(std::cin, user_filename);
                save_to_file(config, user_filename);
                std::cout << "File saved" << std::endl;
                break;
            }
            case 'q': return 0;
                default: std::cerr << "Invalid input" << std::endl;
                return 1;
        }
    }

    return 0;
}
