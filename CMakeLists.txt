# include some defines automatically made by qpm
include(qpm_defines.cmake)

# override mod id
# set(MOD_ID "QuestSounds")

# Enable link time optimization
# In my experience, this can be highly unstable but it nets a huge size optimization and likely performance
# However, the instability was seen using Android.mk/ndk-build builds. With Ninja + CMake, this problem seems to have been solved.
# As always, test thoroughly
# - Fern
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

cmake_minimum_required(VERSION 3.21)
project(${COMPILE_ID})
include(${EXTERN_DIR}/includes/kaleb/shared/cmake/assets.cmake)

# c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# define that stores the actual source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# compile options used
add_compile_options(-frtti -fexceptions -fvisibility=hidden)
add_compile_options(-Ofast)
# compile definitions used
add_compile_definitions(VERSION=\"${MOD_VERSION}\")
add_compile_definitions(MOD_ID=\"${MOD_ID}\")
add_compile_definitions(VERSION_LONG=0)
add_compile_definitions(USE_CODEGEN_FIELDS)

# BeatTogether compile definitions
message(STATUS "Using Server http://master.beattogether.systems:8989 with StatusUrl \"http://master.beattogether.systems/status\"")
if (DEFINED GRAPH_URL AND DEFINED STATUS_URL AND DEFINED SERVER_NAME)
    add_compile_definitions(GRAPH_URL=\"${GRAPH_URL}\")
    add_compile_definitions(STATUS_URL=\"${STATUS_URL}\")
    add_compile_definitions(SERVER_NAME=\"${SERVER_NAME}\")
        message(AUTHOR_WARNING "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nOverriding default with ${SERVER_NAME} ${GRAPH_URL} with StatusUrl ${STATUS_URL}\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
endif()


# recursively get all src files
file(GLOB_RECURSE cpp_file_list ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE c_file_list ${SOURCE_DIR}/*.c)

file(GLOB_RECURSE inline_hook_c ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
file(GLOB_RECURSE inline_hook_cpp ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)

# add all src files to compile
add_library(
        ${COMPILE_ID}
        SHARED
        ${cpp_file_list}
        ${c_file_list}
        ${inline_hook_c}
        ${inline_hook_cpp}
)

target_include_directories(${COMPILE_ID} PRIVATE .)

# add src dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${SOURCE_DIR})
# add include dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${INCLUDE_DIR})
# add shared dir as include dir
target_include_directories(${COMPILE_ID} PUBLIC ${SHARED_DIR})

add_assets(${COMPILE_ID}-assets STATIC ${CMAKE_CURRENT_LIST_DIR}/assets ${INCLUDE_DIR}/assets.hpp)
target_link_libraries(${COMPILE_ID} PRIVATE -llog ${COMPILE_ID}-assets)
# add extern stuff like libs and other includes
include(extern.cmake)

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_STRIP} -d --strip-all
        "lib${COMPILE_ID}.so" -o "stripped_lib${COMPILE_ID}.so"
        COMMENT "Strip debug symbols done on final binary.")

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory debug
        COMMENT "Make directory for debug symbols"
        )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename lib${COMPILE_ID}.so debug/lib${COMPILE_ID}.so
        COMMENT "Rename the lib to debug_ since it has debug symbols"
        )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename stripped_lib${COMPILE_ID}.so lib${COMPILE_ID}.so
        COMMENT "Rename the stripped lib to regular"
        )
