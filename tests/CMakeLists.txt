cmake_minimum_required(VERSION 3.14)

add_library(test_lib TestMemtable.cpp TestSST.cpp TestDb.cpp TestExtendibleHashtable.cpp TestBase.h TestUtils.cpp TestLRU.cpp TestLSMTree.cpp TestBloomFilter.cpp TestClock.cpp)
target_link_libraries(test_lib db)

add_executable(test TestRunner.cpp)
target_link_libraries(test test_lib)
