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

# c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

# define that stores the actual source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# compile options used
add_compile_options(-frtti -fexceptions)
add_compile_options(-Ofast)
# compile definitions used
add_compile_definitions(VERSION=\"${MOD_VERSION}\")
add_compile_definitions(ID=\"${MOD_ID}\")
add_compile_definitions(MOD_ID=\"${MOD_ID}\")

# BeatTogether compile definitions
if (NOT DEFINED HOST_NAME OR NOT DEFINED STATUS_URL)
    add_compile_definitions(HOST_NAME=\"master.beattogether.systems\")
    add_compile_definitions(PORT=2328)
    add_compile_definitions(STATUS_URL=\"http://master.beattogether.systems/status\")
	message(STATUS "Using Server master.beattogether.systems:2328 with StatusUrl \"http://master.beattogether.systems/status\"")
elseif (NOT DEFINED ${PORT})
    add_compile_definitions(HOST_NAME=\"${HOST_NAME}\")
    add_compile_definitions(STATUS_URL=\"${STATUS_URL}\")
    add_compile_definitions(PORT=2328)
    message(AUTHOR_WARNING "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nOverriding default Server to ${HOST_NAME}:2328 with StatusUrl ${STATUS_URL}\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
else()
    add_compile_definitions(HOST_NAME=\"${HOST_NAME}\")
    add_compile_definitions(STATUS_URL=\"${STATUS_URL}\")
    add_compile_definitions(PORT=${PORT})
    message(AUTHOR_WARNING "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nOverriding default Server to  ${HOST_NAME}:${PORT} with StatusUrl ${STATUS_URL}\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
endif()


# recursively get all src files
RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list ${SOURCE_DIR}/*.c)

# add all src files to compile
add_library(
        ${COMPILE_ID}
        SHARED
        ${cpp_file_list}
        ${c_file_list}
)

target_include_directories(${COMPILE_ID} PRIVATE .)

target_include_directories(${COMPILE_ID} PRIVATE extern/includes/questui_components)

# add src dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${SOURCE_DIR})
# add include dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${INCLUDE_DIR})
# add shared dir as include dir
target_include_directories(${COMPILE_ID} PUBLIC ${SHARED_DIR})
# codegen includes 
target_include_directories(${COMPILE_ID} PRIVATE ${EXTERN_DIR}/includes/${CODEGEN_ID}/include)

target_link_libraries(${COMPILE_ID} PRIVATE -llog)
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