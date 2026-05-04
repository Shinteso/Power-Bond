project(utils)

add_library(utils
        vec.cpp vec.h
        physics.cpp physics.h
        toggle.cpp toggle.h
        random.cpp random.h
)

target_link_libraries(utils PUBLIC SDL3::SDL3)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})