add_library(ResEmbed STATIC)

target_sources(ResEmbed PRIVATE
        ResEmbed/Entries.cpp
        ResEmbed/Entries.h
        ResEmbed/InternalStorage.cpp
        ResEmbed/InternalStorage.h
        ResEmbed/ResEmbed.cpp
        ResEmbed/ResEmbed.h
        )

target_include_directories(ResEmbed PUBLIC ${CMAKE_CURRENT_LIST_DIR})
