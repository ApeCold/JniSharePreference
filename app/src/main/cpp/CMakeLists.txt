cmake_minimum_required(VERSION 3.4.1)



add_library(
        native-lib
        SHARED
        native-lib.cpp JniUtils.cpp JniUtils.h)

target_link_libraries( 
        native-lib
        log)