#include "Tools/Utils/String.h"

using namespace Tools::Utils;

bool String::replace(std::string& pTarget, const std::string& pFrom, const std::string& pTo)
{
	size_t startPos = pTarget.find(pFrom);

	if (startPos != std::string::npos)
	{
		pTarget.replace(startPos, pFrom.length(), pTo);
		return true;
	}

	return false;
}

void String::replaceAll(std::string& pTarget, const std::string& pFrom, const std::string& pTo)
{
	if (pFrom.empty()) 
        return;

	size_t startPos = 0;
	while ((startPos = pTarget.find(pFrom, startPos)) != std::string::npos)
	{
		pTarget.replace(startPos, pFrom.length(), pTo);
		startPos += pTo.length();
	}
}

std::string String::generateUnique(const std::string& pSource, std::function<bool(std::string)> pIsAvailable)
{
    std::string suffixlessSource = pSource;

    size_t suffixOpeningParenthesisPos = std::string::npos;
    size_t suffixClosingParenthesisPos = std::string::npos;

    size_t currentPos = decltype(std::string::npos){ pSource.length() - 1 };

    for (auto it = pSource.rbegin(); it < pSource.rend(); ++it, --currentPos)
    {
        const auto c = *it;

        if (suffixClosingParenthesisPos == std::string::npos && c == ')') 
            suffixClosingParenthesisPos = currentPos;

        if (suffixClosingParenthesisPos != std::string::npos && c == '(') 
            suffixOpeningParenthesisPos = currentPos;
    }

    uint32_t counter = uint32_t{ 1 };

    if (suffixOpeningParenthesisPos != std::string::npos && suffixClosingParenthesisPos == pSource.length() - 1 && suffixOpeningParenthesisPos > 0 && pSource[suffixOpeningParenthesisPos - 1] == ' ')
    {
        const std::string between = pSource.substr(suffixOpeningParenthesisPos + 1, suffixClosingParenthesisPos - suffixOpeningParenthesisPos - 1);

        if (!between.empty() && std::find_if(between.begin(), between.end(), [](unsigned char c) { return !std::isdigit(c); }) == between.end())
        {
            counter = static_cast<uint32_t>(std::atoi(between.c_str()));
            suffixlessSource = pSource.substr(0, suffixOpeningParenthesisPos - 1);
        }
    }

    std::string result = suffixlessSource;

    while (!pIsAvailable(result))
    {
        result = suffixlessSource + " (" + std::to_string(counter++) + ")";
    }

    return result;
}