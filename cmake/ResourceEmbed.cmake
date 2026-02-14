function(embed_resources TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "CATEGORY" "")

    if(NOT ARG_CATEGORY)
        set(ARG_CATEGORY "Resources")
    endif()

    foreach(INPUT_FILE IN LISTS ARG_UNPARSED_ARGUMENTS)
        cmake_path(IS_ABSOLUTE INPUT_FILE IS_ABS)
        if(IS_ABS)
            set(ABSOLUTE_INPUT "${INPUT_FILE}")
        else()
            set(ABSOLUTE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE}")
        endif()
        get_filename_component(RESOURCE_NAME "${INPUT_FILE}" NAME_WE)
        set(OUTPUT_CPP "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_NAME}_resource.cpp")

        add_custom_command(
            OUTPUT "${OUTPUT_CPP}"
            COMMAND ResourceGenerator "${ABSOLUTE_INPUT}" "${OUTPUT_CPP}" "${RESOURCE_NAME}" "${ARG_CATEGORY}"
            DEPENDS "${ABSOLUTE_INPUT}" ResourceGenerator
            COMMENT "Embedding resource ${RESOURCE_NAME} from ${INPUT_FILE} [${ARG_CATEGORY}]"
        )

        target_sources(${TARGET} PRIVATE "${OUTPUT_CPP}")
    endforeach()
endfunction()

function(embed_resource_directory TARGET DIRECTORY)
    cmake_parse_arguments(PARSE_ARGV 2 ARG "" "CATEGORY" "")

    file(GLOB_RECURSE FILES "${DIRECTORY}/*.*")

    foreach(FILE IN LISTS FILES)
        if(ARG_CATEGORY)
            embed_resources(${TARGET} CATEGORY "${ARG_CATEGORY}" ${FILE})
        else()
            embed_resources(${TARGET} ${FILE})
        endif()
    endforeach ()


endfunction()
