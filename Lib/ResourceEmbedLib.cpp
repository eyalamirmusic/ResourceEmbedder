#include "ResourceEmbedLib.h"

namespace Resources
{
CategoryMap& getMap()
{
    static CategoryMap map;
    return map;
}

DataView get(const std::string& name, const std::string& category)
{
    return {getCategory(category)[name]};
}

ResourceMap& getCategory(const std::string& category)
{
    return getMap()[category];
}

Data::Data(const std::string& name, const RawData& rawData, const std::string& category)
{
    getCategory(category)[name] = rawData;
}
} // namespace Resources