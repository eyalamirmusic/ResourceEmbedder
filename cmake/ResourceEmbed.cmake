function(embed_resources TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "CATEGORY;NAMESPACE" "FILES")

    if(NOT ARG_CATEGORY)
        set(ARG_CATEGORY "Resources")
    endif()

    if(NOT ARG_NAMESPACE)
        set(ARG_NAMESPACE "ResourceData")
    endif()

    set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAMESPACE}")
    file(MAKE_DIRECTORY "${GENERATED_DIR}")

    set(HEADER_FILE "${GENERATED_DIR}/${ARG_NAMESPACE}.h")
    set(ENTRIES_CPP "${GENERATED_DIR}/${ARG_NAMESPACE}.cpp")
    set(GENERATED_C_FILES "")
    set(ABSOLUTE_FILES "")

    set(FILE_INDEX 0)
    foreach(INPUT_FILE IN LISTS ARG_FILES)
        cmake_path(IS_ABSOLUTE INPUT_FILE IS_ABS)
        if(IS_ABS)
            set(ABSOLUTE_INPUT "${INPUT_FILE}")
        else()
            set(ABSOLUTE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE}")
        endif()

        set(OUTPUT_C "${GENERATED_DIR}/BinaryResource${FILE_INDEX}.c")

        add_custom_command(
            OUTPUT "${OUTPUT_C}"
            COMMAND ResourceGenerator embed "${ABSOLUTE_INPUT}" "${OUTPUT_C}" "${FILE_INDEX}"
            DEPENDS "${ABSOLUTE_INPUT}" ResourceGenerator
            COMMENT "Embedding ${INPUT_FILE}"
        )

        list(APPEND ABSOLUTE_FILES "${ABSOLUTE_INPUT}")
        list(APPEND GENERATED_C_FILES "${OUTPUT_C}")
        math(EXPR FILE_INDEX "${FILE_INDEX} + 1")
    endforeach()

    list(JOIN ABSOLUTE_FILES "," FILES_CSV)

    add_custom_command(
        OUTPUT "${HEADER_FILE}" "${ENTRIES_CPP}"
        COMMAND ResourceGenerator init "${GENERATED_DIR}" "${ARG_NAMESPACE}" "${ARG_CATEGORY}" "${FILES_CSV}"
        DEPENDS ResourceGenerator
        COMMENT "Generating ${ARG_NAMESPACE}.h and ${ARG_NAMESPACE}.cpp"
    )

    get_target_property(INIT_ADDED ${TARGET} RESOURCE_EMBED_INIT_ADDED)
    if(NOT INIT_ADDED)
        target_include_directories(${TARGET} PUBLIC "${GENERATED_DIR}")
        target_link_libraries(${TARGET} PUBLIC ResourceEmbedLib)
        set_target_properties(${TARGET} PROPERTIES RESOURCE_EMBED_INIT_ADDED TRUE)
    endif()

    target_sources(${TARGET} PRIVATE ${GENERATED_C_FILES} "${ENTRIES_CPP}" "${HEADER_FILE}")
endfunction()

function(embed_resource_directory TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "CATEGORY;DIRECTORY;NAMESPACE" "")

    file(GLOB_RECURSE FILES "${ARG_DIRECTORY}/*.*")

    set(FORWARD_ARGS FILES ${FILES})
    if(DEFINED ARG_CATEGORY)
        list(APPEND FORWARD_ARGS CATEGORY "${ARG_CATEGORY}")
    endif()
    if(DEFINED ARG_NAMESPACE)
        list(APPEND FORWARD_ARGS NAMESPACE "${ARG_NAMESPACE}")
    endif()

    embed_resources(${TARGET} ${FORWARD_ARGS})
endfunction()
