#pragma once

#include <string>
#include <functional>


namespace Tools::Utils
{
	class String
	{
	public:
		
		String() = delete;

		static bool replace(std::string& pTarget, const std::string& pFrom, const std::string& pTo);

		static void replaceAll(std::string& pTarget, const std::string& pFrom, const std::string& pTo);

        static std::string generateUnique(const std::string& pSource, std::function<bool(std::string)> pIsAvailable);


		template <typename T> static const T& arg(const T& value);

		template <typename T> static const char* arg(const std::basic_string<T>& value);

		template <typename ... T> static std::string formatString(const char* str, const T ... args);
	};

	template <typename T> const T& String::arg(const T& value)
	{
		return value;
	}

	template <typename T> const char* String::arg(const std::basic_string<T>& value)
	{
		return value.c_str();
	}

	template <typename ... T> std::string String::formatString(const char* str, const T ... args)
	{
		int size = std::snprintf(nullptr, 0, str, arg(args) ...) + 1;
		char* buffer = new char[size];
		
		sprintf_s(buffer, size, str, arg(args) ...);
		std::string fStr(buffer);
		
		delete[] buffer;
		return fStr;
	}
}