#pragma once

#include <vector>
#include <map>
#include <span>
#include <string>

namespace Resources
{
    using DataView = std::span<const unsigned char>;
    using Storage = std::vector<unsigned char>;
    using Map = std::map<std::string, Storage>;
    using RawData = std::initializer_list<unsigned char>;

    DataView get(const std::string& name);

    struct Data
    {
        Data(const std::string& name, const RawData& rawData);
    };
} // namespace Resources
