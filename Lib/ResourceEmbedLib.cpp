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

void registerEntries(const ResourceEntry* entries, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        auto& entry = entries[i];
        getCategory(entry.category)[entry.name] = View(entry.data, entry.size);
    }
}
} // namespace Resources
