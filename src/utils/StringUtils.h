#pragma once
#include <string>
#include <algorithm>
#include <cctype>

class StringUtils {
public:
    // Convert to Title Case 
    static std::string toTitleCase(const std::string& str) {
        if (str.empty()) return str;
        
        std::string result = str;
        // Trims the whitespace
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        
        // Converts to lowercase first
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Capitalize first letter and then letters after spaces
        bool capitalizeNext = true;
        for (char& c : result) {
            if (std::isspace(c)) {
                capitalizeNext = true;
            } else if (capitalizeNext) {
                c = std::toupper(c);
                capitalizeNext = false;
            }
        }
        
        return result;
    }
    
    // Converts to uppercase 
    static std::string toUpperCase(const std::string& str) {
        if (str.empty()) return str;
        
        std::string result = str;
        // Trims the whitespace
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
};