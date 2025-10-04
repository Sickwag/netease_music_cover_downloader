#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "cover_downloader.h"
#include "utils.h"

int chooseFilenameFormat() {
    std::cout << "\nPlease select cover naming format:\n";
    std::cout << "1. Artist - Song\n";
    std::cout << "2. Song - Artist\n";
    std::cout << "3. Index.Artist - Song\n";
    std::cout << "4. Index.Song - Artist\n";

    std::string choice;
    std::cout << "Enter number (1-4), default 1: ";
    std::getline(std::cin, choice);

    if (choice == "1" || choice == "2" || choice == "3" || choice == "4") {
        return std::stoi(choice);
    }
    return 1; // Default value
}

int main() {
    // Enable UTF-8 support for console output
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "Please enter multiple NetEase Music song or album links/IDs (one per line), press enter with empty line to confirm:\n";

    std::vector<std::string> userInput;
    std::string line;

    try {
        while (true) {
            std::getline(std::cin, line);
            if (line.empty()) {
                break;
            }
            userInput.push_back(line);
        }

        int formatType = chooseFilenameFormat();

        CoverDownloader downloader;
        std::vector<std::string> failList;

        std::cout << "Starting to process " << userInput.size() << " items...\n";
        for (size_t idx = 0; idx < userInput.size(); ++idx) {
            downloader.downloadCover(userInput[idx], formatType, static_cast<int>(idx + 1), failList);
        }

        // Output processing result statistics
        int successCount = static_cast<int>(userInput.size()) - static_cast<int>(failList.size());
        std::cout << "Processing completed! Success: " << successCount << ", Failed: " << failList.size() << std::endl;

        if (!failList.empty()) {
            std::cout << "Failed list:\n";
            for (size_t i = 0; i < failList.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << failList[i] << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cout << "\nError occurred: " << e.what() << std::endl;
    }

    return 0;
}
