#ifndef KNULLI_CORE_CONFIG_H
#define KNULLI_CORE_CONFIG_H

#include <string>
#include <map>
#include <memory>

namespace knulli {
namespace core {

class Config {
public:
    static Config& getInstance();
    
    bool load(const std::string& configPath);
    bool save(const std::string& configPath);
    
    // Getters
    std::string getString(const std::string& key, const std::string& defaultValue = "");
    int getInt(const std::string& key, int defaultValue = 0);
    bool getBool(const std::string& key, bool defaultValue = false);
    float getFloat(const std::string& key, float defaultValue = 0.0f);
    
    // Setters
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setBool(const std::string& key, bool value);
    void setFloat(const std::string& key, float value);
    
    // Knulli-specific config paths
    static std::string getKnulliConfigDir();
    static std::string getDefaultConfigPath();
    
    void reset();

private:
    Config();
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    std::map<std::string, std::string> configMap;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_CONFIG_H
