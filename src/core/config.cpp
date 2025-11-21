#include "config.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

namespace knulli {
namespace core {

namespace fs = std::filesystem;

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

Config::Config() {
    // Load default configuration
    setInt("max_history_size", 1000);
    setInt("display_refresh_rate", 60);
    setInt("input_polling_rate", 100);
    setBool("enable_logging", true);
    setBool("enable_button_mapping", true);
    setString("default_button_scheme", "knulli");
}

Config::~Config() {
}

std::string Config::getKnulliConfigDir() {
    const char* userdata = std::getenv("KNULLI_USERDATA");
    if (userdata) {
        return std::string(userdata) + "/system/configs/input-tester";
    }
    // Fallback for development/testing
    return "/tmp/knulli-input-tester";
}

std::string Config::getDefaultConfigPath() {
    return getKnulliConfigDir() + "/config.ini";
}

bool Config::load(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        Logger::getInstance().warning("Config file not found: " + configPath);
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Handle sections [section]
        if (line[0] == '[' && line[line.length() - 1] == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t delimPos = line.find('=');
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            
            // Trim whitespace
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            
            // Add section prefix to key if in section
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            configMap[key] = value;
        }
    }
    
    file.close();
    Logger::getInstance().info("Config loaded from: " + configPath);
    return true;
}

bool Config::save(const std::string& configPath) {
    // Create directory if it doesn't exist
    fs::path configFile(configPath);
    fs::create_directories(configFile.parent_path());
    
    std::ofstream file(configPath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to open config file for writing: " + configPath);
        return false;
    }
    
    file << "; Knulli Input Tester Configuration\n";
    file << "; Auto-generated - edit with care\n\n";
    file << "[general]\n";
    
    for (const auto& [key, value] : configMap) {
        // Skip sections (they'll be handled by key naming)
        size_t dotPos = key.find('.');
        if (dotPos == std::string::npos) {
            file << key << "=" << value << "\n";
        }
    }
    
    file.close();
    Logger::getInstance().info("Config saved to: " + configPath);
    return true;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            Logger::getInstance().warning("Failed to parse int for key: " + key);
        }
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        std::string value = it->second;
        return value == "true" || value == "1" || value == "yes" || value == "on";
    }
    return defaultValue;
}

float Config::getFloat(const std::string& key, float defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            Logger::getInstance().warning("Failed to parse float for key: " + key);
        }
    }
    return defaultValue;
}

void Config::setString(const std::string& key, const std::string& value) {
    configMap[key] = value;
}

void Config::setInt(const std::string& key, int value) {
    configMap[key] = std::to_string(value);
}

void Config::setBool(const std::string& key, bool value) {
    configMap[key] = value ? "true" : "false";
}

void Config::setFloat(const std::string& key, float value) {
    configMap[key] = std::to_string(value);
}

void Config::reset() {
    configMap.clear();
    // Re-initialize with defaults
    setInt("max_history_size", 1000);
    setInt("display_refresh_rate", 60);
    setInt("input_polling_rate", 100);
    setBool("enable_logging", true);
    setBool("enable_button_mapping", true);
    setString("default_button_scheme", "knulli");
}

} // namespace core
} // namespace knulli
