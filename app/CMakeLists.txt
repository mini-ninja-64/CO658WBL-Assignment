cmake_minimum_required (VERSION 3.15)

setupExecutableModule("App")

setupExecutableModule("DB-Benchmark")
target_compile_options("DB-Benchmark" PRIVATE "-DBENCHMARK_DB")

find_package(Boost 1.82.0 REQUIRED)

if(Boost_FOUND)
    target_include_directories("App" PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories("App-test" PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories("DB-Benchmark" PUBLIC ${Boost_INCLUDE_DIRS})
endif()
