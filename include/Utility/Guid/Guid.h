#pragma once

namespace bc
{
	class Guid
	{
	public:
		Guid() {}
		~Guid() {}

		static std::string CreateGuId();
		static bool CreateGuId(std::string& serialNum, std::string strPrefix = "");
		static bool CreateGuIdByTime(std::string& serialNum, std::string strPrefix);
	};
}
