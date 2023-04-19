cmake_minimum_required(VERSION 3.14)

set_target_properties(PROPERTIES LINKER_LANGUAGE CXX)

add_library(db Db.cpp Memtable.cpp SST.cpp RedBlackTree.cpp BufferPool.cpp Bucket.cpp ExtendibleHashtable.cpp LRU.cpp Clock.cpp ../include/Utils.h Utils.cpp LSMTree.cpp Level.cpp BloomFilter.cpp InputReader.cpp ScanInputReader.cpp OutputWriter.cpp)
target_include_directories(db PUBLIC ../include)
target_link_libraries(db PUBLIC ${CMAKE_SOURCE_DIR}/lib/libxxhash.a)
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(CSC443_project main.cpp)
target_link_libraries(CSC443_project PUBLIC db)
