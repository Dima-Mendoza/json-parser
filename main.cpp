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

struct ParsedLine {
    LineType type;
    std::string key;
    std::string value;
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
        if (line[0] != '{' or line[0] != '[') return false;
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
    std::string value = trimmed_line.substr(pos+1);

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
    find_first and find_last can return minus statament and it give UB!!!
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

std::map<std::string, std::string>parse(const std::vector<std::string> &lines) {
    std::map<std::string, std::string> result;

    for (const auto& line : lines) {
        ParsedLine parsed_line = parse_line(line);
        if (parsed_line.type == LineType::KEY_VALUE) {
            result[parsed_line.key] =parsed_line.value;
        }
    }

    return result;
}



int main() {

    std::vector<std::string> test_lines = {
        R"("name": "John")",
        R"("age": 25)",
        R"(isAdmin: true)",
        R"(country: "USA")",
        R"(empty: "")",
        R"(  "withSpaces"  :    "  spaced value  "  )"
    };

    std::map<std::string, std::string> test = parse(test_lines);

    for (const auto& [key, value] : test) {
        std::cout<< key << std::endl;
        std::cout << value << std::endl;
    }

    // for (const auto& line : test_lines) {
    //     ParsedLine parsed = parse_line(line);
    //     std::cout << "Line: " << line << "\n";
    //     std::cout << "  Type: " << to_string(parsed.type) << "\n";
    //     std::cout << "  Key: [" << parsed.key << "]\n";
    //     std::cout << "  Value: [" << parsed.value << "]\n\n";
    // }
    // const std::string test_data = "Lol:true";
    //
    // parse_line(test_data);

    // std::string filename;
    // std::getline(std::cin, filename);
    // load_file(filename);
    // std::cout << is_json(filename);

    return 0;
}
