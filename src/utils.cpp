#include "utils.h"
#include <cstdlib>
#include <ctime>

// Generate random user agent
std::string Utils::getRandomUserAgent() {
    static std::vector<std::string> userAgents = {
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, userAgents.size() - 1);

    return userAgents[dis(gen)];
}

// Generate random delay
double Utils::getRandomDelay(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(min, max);

    return dis(gen);
}

// Extract ID and type (song or album) from user input
std::pair<std::string, std::string> Utils::extractIdAndType(const std::string& input) {
    if (input.find("music.163.com") != std::string::npos) {
        // Handle both URL formats: with # and without #
        std::regex song_regex(R"(song\?id=(\d+))");
        std::smatch song_match;
        if (std::regex_search(input, song_match, song_regex)) {
            return std::make_pair(song_match[1].str(), "song");
        }

        // Also check for #/song?id= format
        std::regex song_regex_hash(R"(#/song\?id=(\d+))");
        if (std::regex_search(input, song_match, song_regex_hash)) {
            return std::make_pair(song_match[1].str(), "song");
        }

        std::regex album_regex(R"(album\?id=(\d+))");
        std::smatch album_match;
        if (std::regex_search(input, album_match, album_regex)) {
            return std::make_pair(album_match[1].str(), "album");
        }

        // Also check for #/album?id= format
        std::regex album_regex_hash(R"(#/album\?id=(\d+))");
        if (std::regex_search(input, album_match, album_regex_hash)) {
            return std::make_pair(album_match[1].str(), "album");
        }
    } else {
        // Assume input is ID, default to song
        return std::make_pair(input, "song");
    }

    return std::make_pair("", "");
}

// Sanitize invalid characters in filename
std::string Utils::sanitizeFilename(const std::string& name) {
    std::string result = name;

    // Replace invalid characters in filename for Windows
    for (auto& c : result) {
        if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }

    // Remove leading and trailing whitespace
    size_t start = result.find_first_not_of(" \t\n\r");
    size_t end = result.find_last_not_of(" \t\n\r");

    if (start == std::string::npos) {
        return "";
    }

    std::string cleaned = result.substr(start, end - start + 1);

    // Additional cleaning for problematic characters
    std::string final_result;
    for (char c : cleaned) {
        // Skip control characters - cast to unsigned char to properly handle UTF-8
        if (static_cast<unsigned char>(c) >= 32) {
            final_result += c;
        } else {
            final_result += '_';
        }
    }

    // Limit filename length to avoid issues
    if (final_result.length() > 100) {
        final_result = final_result.substr(0, 100);
    }

    return final_result;
}

// Generate filename
std::string Utils::generateFilename(int index, const std::string& name1, const std::string& name2, int formatType) {
    std::string name;

    switch (formatType) {
        case 1:
            name = name1 + " - " + name2;
            break;
        case 2:
            name = name2 + " - " + name1;
            break;
        case 3:
            name = (index < 10 ? "0" : "") + std::to_string(index) + "." + name1 + " - " + name2;
            break;
        case 4:
            name = (index < 10 ? "0" : "") + std::to_string(index) + "." + name2 + " - " + name1;
            break;
        default:
            name = name1 + " - " + name2;
            break;
    }

    return sanitizeFilename(name) + ".jpg";
}
