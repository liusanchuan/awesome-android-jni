#include <jni.h>
#include <string>
#include "include/caffe_jni.h"
//#include "caffe/caffe.hpp"
//#include "caffe/layers/memory_data_layer.hpp"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_caffe_1mobile_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {


//    caffe::Caffe ;
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_setNumThreads(JNIEnv *env, jobject thiz,
                                                         jint num_threads) {

    Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setNumThreads(env,thiz, num_threads);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_enableLog(JNIEnv *env, jobject thiz, jboolean enabled) {
    // TODO: implement enableLog()
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_loadModel(JNIEnv *env, jobject thiz, jstring model_path,
                                                     jstring weights_path) {
    // TODO: implement loadModel()
    return Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_loadModel(env,  thiz, model_path, weights_path);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_setMeanWithMeanFile(JNIEnv *env, jobject thiz,
                                                               jstring mean_file) {
    // TODO: implement setMeanWithMeanFile()
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_setMeanWithMeanValues(JNIEnv *env, jobject thiz,
                                                                 jfloatArray mean_values) {
    // TODO: implement setMeanWithMeanValues()
    Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setMeanWithMeanValues(env,thiz,mean_values);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_setScale(JNIEnv *env, jobject thiz, jfloat scale) {
    // TODO: implement setScale()
}extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_getConfidenceScore(JNIEnv *env, jobject thiz,
                                                              jbyteArray data, jint width,
                                                              jint height) {
    // TODO: implement getConfidenceScore()
}extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_predictImage(JNIEnv *env, jobject thiz, jbyteArray data,
                                                        jint width, jint height, jint k) {
    // TODO: implement predictImage()
    return Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_predictImage(env,thiz,data,width,height,k);
}
extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_caffe_1mobile_CaffeMobile_extractFeatures(JNIEnv *env, jobject thiz,
                                                           jbyteArray data, jint width, jint height,
                                                           jstring blob_names) {
    // TODO: implement extractFeatures()
}