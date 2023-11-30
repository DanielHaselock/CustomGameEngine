#pragma once

#include <string>

namespace Tools::Time
{
	class Date
	{
		public:
			Date() = delete;
			static std::string getDateAsString();
			static std::string getTimeAsString();
	};
}