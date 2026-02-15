#pragma once

#include "ResourceEmbedLib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char resource_0_data[];
extern const size_t resource_0_size;

#ifdef __cplusplus
}
#endif

static const ResourceEntry resource_entries[] = {
    {"data.txt", "Resources", resource_0_data, resource_0_size},
};

static const size_t resource_entry_count = sizeof(resource_entries) / sizeof(ResourceEntry);
