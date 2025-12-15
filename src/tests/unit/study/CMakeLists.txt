target_sources(all_tests PRIVATE
    math.cpp
    test_math.cpp
)

target_include_directories(all_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
