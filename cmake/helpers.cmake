function(setup_google_test)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        f53219cdcb7b084ef57414efea92ee5b71989558
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
endfunction()

function(setupExecutableModule name)
    project("${name}")
    set(PROJECT_EXECUTABLE "${PROJECT_NAME}")
    set(TEST_EXECUTABLE "${PROJECT_NAME}-test")

    file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")
    file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "test/*.cpp")

    add_executable("${PROJECT_EXECUTABLE}" ${SOURCE_FILES})
    add_executable("${TEST_EXECUTABLE}" ${SOURCE_FILES} ${TEST_FILES})

    target_include_directories("${PROJECT_EXECUTABLE}" PRIVATE "include")
    target_include_directories("${TEST_EXECUTABLE}" PRIVATE "include")

    target_compile_options(${PROJECT_EXECUTABLE} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(${TEST_EXECUTABLE} PRIVATE -DTESTING -Wall -Wextra -Wpedantic -Werror)

    target_link_libraries("${TEST_EXECUTABLE}" gtest_main gmock)
    include(GoogleTest)
    gtest_discover_tests("${TEST_EXECUTABLE}")
endfunction()
