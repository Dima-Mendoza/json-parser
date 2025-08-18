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

struct ScanState {
    int obj = 0;
    int arr = 0;
    bool inStr = false;
    bool esc = false;
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

std::string to_string(LineType type) {
    switch (type) {
            case LineType::KEY_VALUE: return "KEY_VALUE";
            case LineType::EMPTY: return "EMPTY";
            case LineType::UNKNOWN: return "UNKNOWN";
    }
}

void advance_state(ScanState& state, char ch) {
    if (state.inStr) {
        if (state.esc) {
            state.esc = false;
            return;
        }
        if (ch == '\\') {
            state.esc = true;
            return;
        }
        if (ch == '"') {
            state.inStr = false;
            return;
        }
        return;
    }

    if (ch == '"') {
        state.inStr = true;
        state.esc = false;
        return;
    }

    switch (ch) {
        case '{': ++state.obj; break;
        case '}': if (state.obj > 0) --state.obj; break;
        case '[': ++state.arr; break;
        case ']': if (state.arr > 0) --state.arr; break;
        default: /*ignore*/ break;
    }

}

int find_colon_outside_string(const std::string& s) {
    ScanState state;
    for (size_t i = 0; i < s.size(); ++i) {
        char ch = s[i];
        if (!state.inStr && state.obj == 0 && state.arr == 0 && ch == ':') {
            return static_cast<int>(i);
        }
        advance_state(state, ch);
    }
    return -1;
}

std::vector<std::pair<std::string, std::string>>
collect_top_level_pairs(const std::vector<std::string>& lines) {

    
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

    auto pairs = collect_top_level_pairs(lines);
    for (auto& [key, raw_value] : pairs) {
        result[key] = detect_type(raw_value);
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
