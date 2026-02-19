#pragma once

#include "ResourceEmbedLib.h"

namespace ResEmbed
{
const Entries& getResourceEntries();
static const Initializer resourceInitializer {getResourceEntries()};
}