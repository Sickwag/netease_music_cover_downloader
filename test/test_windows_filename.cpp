#include <iostream>
#include <string>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>

// Test function to check Windows filename compatibility
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

int main() {
    // Enable UTF-8 support
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::string test_name = "gomissing - \xE7\x9C\x89 \xE7\x9C\xBC \xE6\xB8\xA9 \xE6\x9F\x94"; // UTF-8 encoded Chinese characters
    std::cout << "Original: " << test_name << std::endl;

    std::string sanitized = sanitizeFilename(test_name);
    std::cout << "Sanitized: " << sanitized << std::endl;

    // Test if we can create a file with this name
    std::string filename = "covers/" + sanitized + ".jpg";
    std::cout << "Full filename: " << filename << std::endl;

    // Create covers directory if it doesn't exist
    CreateDirectoryA("covers", NULL);

    // Try to create the file
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        std::cout << "File created successfully!" << std::endl;
        file.close();
        // Clean up the test file
        DeleteFileA(filename.c_str());
    } else {
        std::cout << "Failed to create file!" << std::endl;
        // Let's try with a simpler name to see what works
        std::string simple_name = "test_file_with_chinese_眉_眼_温_柔.jpg";
        std::ofstream simple_file("covers/" + simple_name, std::ios::binary);
        if (simple_file.is_open()) {
            std::cout << "Simple filename works: " << simple_name << std::endl;
            simple_file.close();
            DeleteFileA(("covers/" + simple_name).c_str());
        } else {
            std::cout << "Even simple filename failed!" << std::endl;
        }
    }

    return 0;
}
