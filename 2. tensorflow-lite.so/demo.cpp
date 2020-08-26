#include <jni.h>
#include <string>
#include <memory>
#include <android/log.h>
#include <iostream>
#include "../include/tensorflow/lite/interpreter.h"
#include "../include/tensorflow/lite/kernels/register.h"

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
    __android_log_print(ANDROID_LOG_ERROR, "hello-jni","Failed with errno %d", &output);

std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jfloat JNICALL
Java_com_example_tflite_1jni2_MainActivity_helloformJni(JNIEnv * env,jobject thiz,jfloat data) {
    // 1. 导入模型，映射到内存
    const char* filename="/sdcard/360/oneone.tflite";
    std::unique_ptr<tflite::FlatBufferModel> model=tflite::FlatBufferModel::BuildFromFile(filename);
    // 2. 构建Tensorflow模型的解释器 interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model.get(),resolver)(&interpreter);
    interpreter->AllocateTensors();
    // 3. 构建输入Tensor
    float* input = interpreter->typed_input_tensor<float >(0);
    input[0]=data;
    // 4. 运行模型，推导结果
    interpreter->Invoke();
    // 5. 获取输出结果
    float* output=interpreter->typed_output_tensor<float >(0);
    return *output;
}
