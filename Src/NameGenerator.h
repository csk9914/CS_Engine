#pragma once

#include <string>
#include <unordered_map>

class NameGenerator
{
public:
    static std::string GenerateUniqueName(const std::string& baseName)
    {
        static std::unordered_map<std::string, int> nameCounts;

        // 해당 이름의 카운트를 올림
        int count = ++nameCounts[baseName];

        // 첫 번째면 그냥 이름 반환, 두 번째부터 숫자 부여
        if (count == 1)
        {
            return baseName;
        }
        else
        {
            return baseName + " (" + std::to_string(count - 1) + ")";
        }
    }
};
