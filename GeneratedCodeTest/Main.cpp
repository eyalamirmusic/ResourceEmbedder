#include "ResourceData.h"
#include <iostream>

int main()
{
    Resources::registerEntries(resource_entries, resource_entry_count);
    std::cout << Resources::get("data.txt").toString() << std::endl;
    return 0;
}
