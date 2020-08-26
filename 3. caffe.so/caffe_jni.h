#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>

// #include <cblas.h>

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>

// #include "caffe/caffe.hpp"
// #include "caffe_mobile.hpp"

#ifdef __cplusplus
extern "C" {
#endif

using std::string;
using std::vector;
//using caffe::CaffeMobile;

// int getTimeSec() {
//   struct timespec now;
//   clock_gettime(CLOCK_MONOTONIC, &now);
//   return (int)now.tv_sec;
// }

string jstring2string(JNIEnv *env, jstring jstr);

/**
 * NOTE: byte[] buf = str.getBytes("US-ASCII")
 */
string bytes2string(JNIEnv *env, jbyteArray buf) ;

//cv::Mat imgbuf2mat(JNIEnv *env, jbyteArray buf, int width, int height)

//cv::Mat getImage(JNIEnv *env, jbyteArray buf, int width, int height)
JNIEXPORT void JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setNumThreads(JNIEnv *env,
                                                             jobject thiz,
                                                             jint numThreads);

JNIEXPORT void JNICALL Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_enableLog(
    JNIEnv *env, jobject thiz, jboolean enabled);

JNIEXPORT jint JNICALL Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_loadModel(
    JNIEnv *env, jobject thiz, jstring modelPath, jstring weightsPath);

JNIEXPORT void JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setMeanWithMeanFile(
    JNIEnv *env, jobject thiz, jstring meanFile);

JNIEXPORT void JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setMeanWithMeanValues(
    JNIEnv *env, jobject thiz, jfloatArray meanValues);

JNIEXPORT void JNICALL Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_setScale(
    JNIEnv *env, jobject thiz, jfloat scale) ;
/**
 * NOTE: when width == 0 && height == 0, buf is a byte array
 * (str.getBytes("US-ASCII")) which contains the img path
 */
JNIEXPORT jfloatArray JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_getConfidenceScore(
    JNIEnv *env, jobject thiz, jbyteArray buf, jint width, jint height);

/**
 * NOTE: when width == 0 && height == 0, buf is a byte array
 * (str.getBytes("US-ASCII")) which contains the img path
 */
JNIEXPORT jintArray JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_predictImage(
    JNIEnv *env, jobject thiz, jbyteArray buf, jint width, jint height,
    jint k);

/**
 * NOTE: when width == 0 && height == 0, buf is a byte array
 * (str.getBytes("US-ASCII")) which contains the img path
 */
JNIEXPORT jobjectArray JNICALL
Java_com_sh1r0_caffe_1android_1lib_CaffeMobile_extractFeatures(
    JNIEnv *env, jobject thiz, jbyteArray buf, jint width, jint height,
    jstring blobNames);

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) ;

#ifdef __cplusplus
}
#endif
