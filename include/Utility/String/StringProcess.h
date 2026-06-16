#pragma once

namespace bc 
{
	extern bool StringStartWith(const std::string& mainstr, const std::string& substr);
	extern void ReplaceAllString(std::string& strSource, const std::string& oldValue, const std::string& newValue);
	extern void SplitStringBySpecial(const std::string& strSource, std::vector<std::string>& vOut, const std::string& strSpecial);
	extern std::string GetExt(const std::string& path);
	extern void GetFileName(std::string& path, std::string& name);
	extern int32_t GetStringSize(const std::string& str);
}

