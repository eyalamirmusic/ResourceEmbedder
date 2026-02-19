#include <Resources.h>
#include <iostream>

int main()
{
    std::cout << ResEmbed::get("Resource.txt").toString();
    return 0;
}