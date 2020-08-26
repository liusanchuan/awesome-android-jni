/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_CONV_BUFFER_1X1_H_
#define TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_CONV_BUFFER_1X1_H_

#include "tensorflow/lite/delegates/gpu/cl/buffer.h"
#include "tensorflow/lite/delegates/gpu/cl/cl_kernel.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/conv_common.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/gpu_operation.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/util.h"
#include "tensorflow/lite/delegates/gpu/cl/linear_storage.h"
#include "tensorflow/lite/delegates/gpu/cl/precision.h"
#include "tensorflow/lite/delegates/gpu/cl/tensor.h"
#include "tensorflow/lite/delegates/gpu/cl/util.h"
#include "tensorflow/lite/delegates/gpu/common/data_type.h"
#include "tensorflow/lite/delegates/gpu/common/operations.h"
#include "tensorflow/lite/delegates/gpu/common/shape.h"
#include "tensorflow/lite/delegates/gpu/common/status.h"
#include "tensorflow/lite/delegates/gpu/common/tensor.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"
#include "tensorflow/lite/delegates/gpu/common/winograd_util.h"

namespace tflite {
namespace gpu {
namespace cl {

class ConvBuffer1x1 : public GPUOperation {
 public:
  ConvBuffer1x1() = default;

  // Move only
  ConvBuffer1x1(ConvBuffer1x1&& operation);
  ConvBuffer1x1& operator=(ConvBuffer1x1&& operation);
  ConvBuffer1x1(const ConvBuffer1x1&) = delete;
  ConvBuffer1x1& operator=(const ConvBuffer1x1&) = delete;

  absl::Status Tune(const TuningParameters& params) override;
  int3 GetGridSize() const override;

  ConvWeightsDescription GetConvWeightsDescription() const {
    ConvWeightsDescription desc;
    desc.layout = ConvWeightsLayout::kOHWIOGroupI4O4;
    desc.output_group_size = conv_params_.block_size.z;
    return desc;
  }

  struct ConvParams {
    int3 block_size = int3(1, 1, 1);
    int element_size = 4;  // can be 4, 8 or 16

    // By default in 2d convolution we have the same weights for WH dims, but in
    // some cases we need separate weights for H dimension and convolution
    // kernel requires very small modifications to support it.
    bool different_weights_for_height = false;
  };

 private:
  ConvBuffer1x1(const OperationDef& definition, const ConvParams& conv_params);
  friend absl::Status CreateConvBuffer1x1(
      const CreationContext& creation_context, const OperationDef& definition,
      const Convolution2DAttributes& attr, ConvBuffer1x1* result,
      const BHWC* shape);
  friend absl::Status CreateConvBuffer1x1(
      const CreationContext& creation_context, const OperationDef& definition,
      const FullyConnectedAttributes& attr, ConvBuffer1x1* result,
      const BHWC* shape);
  friend absl::Status CreateConvBuffer1x1Wino4x4To6x6(
      const CreationContext& creation_context, const OperationDef& definition,
      const Convolution2DAttributes& attr, ConvBuffer1x1* result,
      const BHWC* shape);
  friend absl::Status CreateConvBuffer1x1DynamicWeights(
      const CreationContext& creation_context, const OperationDef& definition,
      const Convolution2DAttributes& attr, const BHWC& weights_shape,
      ConvBuffer1x1* result, const BHWC* dst_shape);

  template <DataType T>
  absl::Status UploadData(const tflite::gpu::Tensor<OHWI, T>& weights,
                          const tflite::gpu::Tensor<Linear, T>& biases,
                          CLContext* context);
  template <DataType T>
  absl::Status UploadDataForWinograd4x4To6x6(
      const tflite::gpu::Tensor<OHWI, T>& weights, const CLDevice& device,
      CLContext* context);

  template <DataType T>
  absl::Status UploadWeights(const tflite::gpu::Tensor<OHWI, T>& weights,
                             CLContext* context);

  template <DataType T>
  absl::Status UploadBiases(const tflite::gpu::Tensor<Linear, T>& biases,
                            CLContext* context);

  std::string GenerateConvBuffer1x1(
      const OperationDef& op_def, const ConvBuffer1x1::ConvParams& conv_params,
      Arguments* args);

  ConvParams conv_params_;
};

template <DataType T>
absl::Status ConvBuffer1x1::UploadData(
    const tflite::gpu::Tensor<OHWI, T>& weights,
    const tflite::gpu::Tensor<Linear, T>& biases, CLContext* context) {
  RETURN_IF_ERROR(UploadWeights(weights, context));
  RETURN_IF_ERROR(UploadBiases(biases, context));
  return absl::OkStatus();
}

template <DataType T>
absl::Status ConvBuffer1x1::UploadDataForWinograd4x4To6x6(
    const tflite::gpu::Tensor<OHWI, T>& weights, const CLDevice& device,
    CLContext* context) {
  tflite::gpu::Tensor<OHWI, T> wino_weights;
  RearrangeWeightsToWinograd4x4To6x6Weights(weights, &wino_weights);
  RETURN_IF_ERROR(UploadWeights(wino_weights, context));
  tflite::gpu::Tensor<Linear, DataType::FLOAT32> bias;
  bias.shape = Linear(weights.shape.o);
  bias.data.resize(weights.shape.o, 0.0f);
  RETURN_IF_ERROR(UploadBiases(bias, context));

  return absl::OkStatus();
}

template <DataType T>
absl::Status ConvBuffer1x1::UploadWeights(
    const tflite::gpu::Tensor<OHWI, T>& weights, CLContext* context) {
  const int dst_depth = DivideRoundUp(weights.shape.o, 4);
  const int src_depth = DivideRoundUp(weights.shape.i, 4);

  const bool f32_weights = definition_.precision == CalculationsPrecision::F32;
  const int float4_size = f32_weights ? sizeof(float4) : sizeof(half4);

  const int dst_depth_aligned = AlignByN(dst_depth, conv_params_.block_size.z);
  const int elements_count =
      weights.shape.h * weights.shape.w * src_depth * dst_depth_aligned * 4;

  Buffer weights_buffer;
  if (f32_weights) {
    std::vector<float4> gpu_data(elements_count);
    RearrangeWeightsToOHWIOGroupI4O4(weights, conv_params_.block_size.z,
                                     absl::MakeSpan(gpu_data));
    RETURN_IF_ERROR(CreateReadOnlyBuffer(float4_size * elements_count,
                                         gpu_data.data(), context,
                                         &weights_buffer));
  } else {
    std::vector<half4> gpu_data(elements_count);
    RearrangeWeightsToOHWIOGroupI4O4(weights, conv_params_.block_size.z,
                                     absl::MakeSpan(gpu_data));
    RETURN_IF_ERROR(CreateReadOnlyBuffer(float4_size * elements_count,
                                         gpu_data.data(), context,
                                         &weights_buffer));
  }

  BufferDescriptor desc;
  desc.element_type = f32_weights ? DataType::FLOAT32 : DataType::FLOAT16;
  desc.element_size = 16;
  desc.memory_type = MemoryType::GLOBAL;

  args_.AddObject("weights", AccessType::READ,
                  absl::make_unique<Buffer>(std::move(weights_buffer)),
                  absl::make_unique<BufferDescriptor>(desc));
  return absl::OkStatus();
}

template <DataType T>
absl::Status ConvBuffer1x1::UploadBiases(
    const tflite::gpu::Tensor<Linear, T>& biases, CLContext* context) {
  TensorLinearDescriptor desc;
  desc.storage_type = LinearStorageType::BUFFER;
  desc.element_type = definition_.GetDataType();

  tflite::gpu::Tensor<Linear, DataType::FLOAT32> bias = biases;
  int channels = AlignByN(biases.shape.v, 4 * conv_params_.block_size.z);
  bias.shape = Linear(channels);
  bias.data.resize(channels, 0.0f);
  LinearStorage lt;
  RETURN_IF_ERROR(CreateLinearStorage(desc, bias, context, &lt));
  args_.AddObject("biases", AccessType::READ,
                  absl::make_unique<LinearStorage>(std::move(lt)),
                  absl::make_unique<TensorLinearDescriptor>(desc));
  return absl::OkStatus();
}

bool IsConvBuffer1x1Supported(const OperationDef& definition,
                              const Convolution2DAttributes& attr);

bool IsConvBuffer1x1Supported(const OperationDef& definition,
                              const BHWC& weights_shape,
                              const Convolution2DAttributes& attr);

absl::Status CreateConvBuffer1x1(const CreationContext& creation_context,
                                 const OperationDef& definition,
                                 const Convolution2DAttributes& attr,
                                 ConvBuffer1x1* result,
                                 const BHWC* shape = nullptr);

absl::Status CreateConvBuffer1x1(const CreationContext& creation_context,
                                 const OperationDef& definition,
                                 const FullyConnectedAttributes& attr,
                                 ConvBuffer1x1* result,
                                 const BHWC* shape = nullptr);

absl::Status CreateConvBuffer1x1DynamicWeights(
    const CreationContext& creation_context, const OperationDef& definition,
    const Convolution2DAttributes& attr, const BHWC& weights_shape,
    ConvBuffer1x1* result, const BHWC* dst_shape = nullptr);

absl::Status CreateConvBuffer1x1Wino4x4To6x6(
    const CreationContext& creation_context, const OperationDef& definition,
    const Convolution2DAttributes& attr, ConvBuffer1x1* result,
    const BHWC* shape = nullptr);

}  // namespace cl
}  // namespace gpu
}  // namespace tflite

#endif  // TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_CONV_BUFFER_1X1_H_
