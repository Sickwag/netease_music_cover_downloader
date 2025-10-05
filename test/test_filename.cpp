#include <iostream>
#include <string>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

// Test function to check UTF-8 handling
std::string sanitizeFilename(const std::string& name) {
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
        // Skip control characters (this is the problem - it's checking ASCII values)
        if (static_cast<unsigned char>(c) >= 32) {  // Cast to unsigned to handle UTF-8 properly
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

int main() {
    // Enable UTF-8 support
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::string test_name = "gomissing - \xE7\x9C\x89 \xE7\x9C\xBC \xE6\xB8\xA9 \xE6\x9F\x94"; // UTF-8 encoded Chinese characters
    std::cout << "Original: " << test_name << std::endl;

    std::string sanitized = sanitizeFilename(test_name);
    std::cout << "Sanitized: " << sanitized << std::endl;

    return 0;
}
