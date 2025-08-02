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

ParsedLine parse_line(const std::string& line) {
    ParsedLine parsed_line;
    LineType type = classify_line(line);
    std::string trimmed_line = trim(line);

    parsed_line.type = type;

    //Create function  auto delete_quotes to delete quotes and add else if line haven't quotes
    if (parsed_line.type == LineType::KEY_VALUE) {
        size_t pos = trimmed_line.find_first_of(':');

        std::string key = trimmed_line.substr(0, pos);
        std::string value = trimmed_line.substr(pos + 1);
        key = trim(key);
        value = trim(value);

        if (key.find_first_of('"') != std::string::npos) {
            parsed_line.key = key.substr(key.find_first_of('"') + 1, key.find_last_of('"') - key.find_first_of('"') - 1);
        }
        if (value.find_first_of('"') != std::string::npos) {
            parsed_line.value = value.substr(value.find_first_of('"') + 1, value.find_last_of('"') - value.find_first_of('"') - 1);
        }


        // if(pos == std::string::npos) {
        //     size_t quote = trimmed_line.find_first_of('"');
        //     size_t quote_end = trimmed_line.find_last_of('"');
        //     if (quote == std::string::npos and quote_end == std::string::npos) {
        //         parsed_line.key = trim(trimmed_line.substr(quote, pos-1));
        //         parsed_line.value = trim(trimmed_line.substr(pos + 1, quote_end));
        //     }
        // }
    }

    return parsed_line;
}



int main() {
    // const std::string test_data = "Lol:true";
    //
    // parse_line(test_data);

    std::string filename;
    std::getline(std::cin, filename);
    load_file(filename);
    std::cout << is_json(filename);

    return 0;
}