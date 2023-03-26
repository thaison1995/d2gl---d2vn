#pragma once
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

namespace d2gl {
class Config {
public:
	void SaveConfig();
	void LoadConfig();

private:
	YAML::Node yaml;

	int GetInt(std::string key, int def, int min, int max);
	float GetFloat(std::string key, float def, float min, float max);
	bool GetBool(std::string key, bool def);
	std::string GetString(std::string key, const std::string& def);
};
}
