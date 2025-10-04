#ifndef NETEASE_MUSIC_COVER_UTILS_H
#define NETEASE_MUSIC_COVER_UTILS_H

#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <regex>

// Some common utility functions

class Utils {
public:
    // Generate random user agent
    static std::string getRandomUserAgent();
    
    // Generate random delay
    static double getRandomDelay(double min = 1.0, double max = 2.0);
    
    // Extract ID and type (song or album) from user input
    static std::pair<std::string, std::string> extractIdAndType(const std::string& input);
    
    // Sanitize invalid characters in filename
    static std::string sanitizeFilename(const std::string& name);
    
    // Generate filename
    static std::string generateFilename(int index, const std::string& name1, 
                                      const std::string& name2, int formatType);
};

#endif // NETEASE_MUSIC_COVER_UTILS_H