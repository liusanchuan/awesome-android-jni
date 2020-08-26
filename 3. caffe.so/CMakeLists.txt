cmake_minimum_required(VERSION 3.4.1)
add_library( # Sets the name of the library.
             native-lib
             SHARED
             demo.cpp
			caffe_jni.h
        )
find_library(
              log-lib
              log )
#  caffe
add_library(sc_caffe
        STATIC
        IMPORTED
        )
set_target_properties(sc_caffe
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/caffe_android_lib/libs/${CMAKE_ANDROID_ARCH_ABI}/libcaffe.so
        )
#  caffe_jni
add_library(sc_caffe_jni
        STATIC
        IMPORTED
        )
set_target_properties(sc_caffe_jni
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/caffe_android_lib/libs/${CMAKE_ANDROID_ARCH_ABI}/libcaffe_jni.so
        )

include_directories(${CMAKE_SOURCE_DIR}/caffe_android_lib/include)

target_link_libraries( native-lib sc_caffe sc_caffe_jni ${log-lib} )