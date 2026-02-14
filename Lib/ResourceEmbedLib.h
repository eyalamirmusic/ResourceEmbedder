#pragma once

#include <vector>
#include <map>
#include <span>
#include <string>

namespace Resources
{
using Storage = std::vector<unsigned char>;
using ResourceMap = std::map<std::string, Storage>;
using CategoryMap = std::map<std::string, ResourceMap>;
using RawData = std::initializer_list<unsigned char>;
using View = std::span<const unsigned char>;

inline constexpr auto DefaultCategory = "Resources";

struct DataView
{
    using Iterator = View::iterator;

    DataView() = default;

    DataView(const View& viewToUse)
        : dataView(viewToUse) {}

    Iterator begin() const { return dataView.begin(); }
    Iterator end() const { return dataView.end(); }

    const void* asRaw() const { return data(); }
    const char* asCharPointer() const { return static_cast<const char*>(asRaw()); }

    const unsigned char* data() const { return dataView.data(); }

    size_t size() const { return dataView.size(); }
    int getSize() const { return static_cast<int>(size()); }

    std::string toString() const { return {asCharPointer(), size()}; }

    View dataView;
};

DataView get(const std::string& name, const std::string& category = DefaultCategory);

ResourceMap& getCategory(const std::string& category);

struct Data
{
    Data(const std::string& name, const RawData& rawData, const std::string& category = DefaultCategory);
};
} // namespace Resources