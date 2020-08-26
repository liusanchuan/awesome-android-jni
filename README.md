# Android JNI开发中常用库汇总

此hub是我在开发android jni 过程中编译或者收集到的一些常用库，android 应用环境比较固定，所以一次编译好，大家就都可以用。（持续更新~ ~）

使用过程遇到问题，欢迎讨论。

[所有文件在Github下载](https://github.com/liusanchuan/awesome-android-jni)
@[toc]
## 1 OpenCV动态库
[github下载](https://github.com/liusanchuan/awesome-android-jni/tree/master/1.%20OpenCV.so)
cmake 导入方法：

~~~shell
cmake_minimum_required(VERSION 3.4.1)
add_library(native-lib
    SHARED
    src/main/cpp/native-lib.cpp
     )

# 1. 添加 opencv库
add_library(sc_opencv
        STATIC
        IMPORTED)
# 2. 链接so
set_target_properties(
        sc_opencv
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jnilibs/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java3.so)
# 3. 链接头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)


target_link_libraries(native-lib
        android
        log
        sc_opencv)
~~~

使用方法：

~~~java
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_myapplication_ImageAnlyness_byteArray2JNI(JNIEnv *env, jclass clazz,jbyteArray input, jint width,jint height) {
    // 读取byte[]转化成CV：：Mat
    jbyte* y_bytes=env->GetByteArrayElements(input,NULL);
    unsigned char* in =(unsigned char* )y_bytes;
    cv::Mat image(height,width,CV_8UC1,(void*)in);
    cv::Mat newImg;
    image.copyTo(newImg);
    time_t now=time(0);
    __android_log_write(ANDROID_LOG_DEBUG,"Camera2apiActivity","3 saved");
    // 保存
    bool isOk= cv::imwrite("/sdcard/360/"+std::string(ctime(&now))+"img.jpeg", newImg);
    if(isOk){
        __android_log_write(ANDROID_LOG_DEBUG,"Camera2apiActivity","4 saved");
    }else{
        __android_log_write(ANDROID_LOG_DEBUG,"Camera2apiActivity","4 not saved");
    }
    jbyteArray array=env->NewByteArray(width*height);
    env->SetByteArrayRegion(array,0,width*height,y_bytes);
    return array;
}
~~~



## 2. tensorflow-lite动态库
[github下载](https://github.com/liusanchuan/awesome-android-jni/tree/master/2.%20tensorflow-lite.so)
tensorflow在手机和嵌入式平台的解释器
cmake导入方法：
~~~shell
# tf lite
add_library(tflite
        STATIC
        IMPORTED
        )
set_target_properties(tflite
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libtensorflowLite.so # 这个路径是从 cmakelists文件到你的so文件的路径
        )
# 下面这三个路径是导入头文件，是从 cmakelists文件到include头文件的位置
include_directories(${CMAKE_SOURCE_DIR}/include/flatbuffers)
include_directories(${CMAKE_SOURCE_DIR}/include/tensorflow)
include_directories(${CMAKE_SOURCE_DIR}/include)
~~~
C++示例：
~~~C++
#include "include/tensorflow/lite/interpreter.h"
#include "include/tensorflow/lite/kernels/register.h"
#include <jni.h>
extern "C" JNIEXPORT jstring
Java_com_example_tflite_1jni2_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    // 1. 导入模型，映射到内存
    const char* filename="/sdcard/360/mnist.tflite";
    std::unique_ptr<tflite::FlatBufferModel> model=tflite::FlatBufferModel::BuildFromFile(filename);
    // 2. 构建Tensorflow模型的解释器 interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model.get(),resolver)(&interpreter);
    interpreter->AllocateTensors();
    // 3. 构建输入Tensor
    float* input = interpreter->typed_input_tensor<float >(0);
    *input=3.3;
    // 4. 运行模型，推导结果
    interpreter->Invoke();
    // 5. 获取输出结果
    float* output=interpreter->typed_output_tensor<float >(0);
    __android_log_print(ANDROID_LOG_ERROR, "hello-jni","output: %d", &output);
}
~~~
## 3. caffe动态库
[github下载](https://github.com/liusanchuan/awesome-android-jni/tree/master/3.%20caffe.so)
这个库的JNI 用法略显诡异，但是诡异是为了简单妥协的。它包括两个库`libcaffe.so`,`libcaffe_jni.so`，后一个是对前一个的JNI层的封装。之所以要用它，是因为它把很多方法帮我们封装起来了，不然caffe还要依赖一大堆库，多大让你头大，我已经大过了。
使用的时候就是用我们自己的JNI代码调用libcaffe_jni.so封装的函数，文件夹内有一个demo。
~~~shell
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
~~~


## 3.5 caffe的aar库
[github下载](https://github.com/liusanchuan/awesome-android-jni/tree/master/3.5%20caffe.aar)
直接把它当做一个普通的jar包使用，很happy。其实就是对上面的库封装了一下，免去了很多步骤。

（1） 把 caffe_android.aar文件复制到新项目的`app/libs`下

（2） 在`build.gradle(app)`中添加引用:

~~~shell
apply plugin: 'com.android.application'

android {
... ...
}
// 这个
repositories {
    flatDir{
        dirs 'libs'
    }
}
dependencies {
    compile (name:'caffe_android',ext:'aar') // 和这个
... ...
}
~~~
## 4. pytorch动态库
还没成功，正在挠头...


## 5. boost静态库
[github下载](https://github.com/liusanchuan/awesome-android-jni/tree/master/5.%20boost.a)
直接用，请看招：
cmake
~~~shell
# Boost
INCLUDE_DIRECTORIES(boost/include)
LINK_DIRECTORIES(boost/lib)      # 静态库 .a 文件目录
~~~

## 6. 未完待续 。。。