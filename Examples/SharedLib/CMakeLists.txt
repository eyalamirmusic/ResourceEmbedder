add_library(Lib STATIC Lib.cpp)
target_link_libraries(Lib PRIVATE ResEmbed)

#This app would use ResourcesA:
add_executable(AppA Main.cpp)
res_embed_add(AppA DIRECTORY ResourcesA)
target_link_libraries(AppA PRIVATE Lib)

#This app would use ResourcesB:
add_executable(AppB Main.cpp)
res_embed_add(AppB DIRECTORY ResourcesB)
target_link_libraries(AppB PRIVATE Lib)

#This app is missing resources, the lib has a fallback for that:
add_executable(AppC MissingResourcesMain.cpp)
target_link_libraries(AppC PRIVATE Lib)