cmake_minimum_required(VERSION 3.14)

add_executable(exp Experiment.h timeUtil.h ExperimentsRunner.cpp)
target_link_libraries(exp db)
