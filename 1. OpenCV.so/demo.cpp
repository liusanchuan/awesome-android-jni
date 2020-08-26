//
// Created by sc1812 on 2020/7/17.
//

#include <jni.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <string>

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