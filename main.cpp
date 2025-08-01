#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>



enum class LineType {
    SECTION,
    KEY_VALUE,
    EMPTY,
    UNKNOWN
};

struct ParsedLine {
    LineType type;
    std::string section;
    std::string key;
    std::string value;
};

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(' ');
    size_t end = str.find_last_not_of(' ');

    if (start != std::string::npos) return "";

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
        if (line[0] != '{') return false;
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
    else if (trimmed_line[0] == '{') return LineType::SECTION;
    else if (trimmed_line.find_first_of(':')) return LineType::KEY_VALUE;
    else return LineType::UNKNOWN;
}

std::string to_string(LineType type) {
    switch (type) {
        case LineType::SECTION: return "SECTION";
            case LineType::KEY_VALUE: return "KEY_VALUE";
            case LineType::EMPTY: return "EMPTY";
            case LineType::UNKNOWN: return "UNKNOWN";
    }
}

ParsedLine parse_line(const std::string& line) {
    ParsedLine parsed_line;
    LineType type = classify_line(line);
    std::string trimmed_line = trim(line);



}



int main() {
    std::string filename;
    std::getline(std::cin, filename);
    load_file(filename);
    std::cout << is_json(filename);

    return 0;
}