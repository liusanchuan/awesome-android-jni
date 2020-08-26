/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#ifndef TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_LSTM_NORMALIZATION_H_
#define TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_LSTM_NORMALIZATION_H_

#include "tensorflow/lite/delegates/gpu/cl/kernels/gpu_operation.h"
#include "tensorflow/lite/delegates/gpu/common/operations.h"
#include "tensorflow/lite/delegates/gpu/common/status.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"

namespace tflite {
namespace gpu {
namespace cl {

// Implements tensor_utils::MeanStddevNormalization
class MeanStdDevNormalization : public GPUOperation {
 public:
  explicit MeanStdDevNormalization(const OperationDef& definition);

  absl::Status Tune(const TuningParameters& params) override {
    return absl::OkStatus();
  }
  int3 GetGridSize() const override;

  // Move only
  MeanStdDevNormalization(MeanStdDevNormalization&& kernel) = default;
  MeanStdDevNormalization& operator=(MeanStdDevNormalization&& kernel) =
      default;
  MeanStdDevNormalization(const MeanStdDevNormalization&) = delete;
  MeanStdDevNormalization& operator=(const MeanStdDevNormalization&) = delete;

 private:
  std::string GetNormalizationCode();
};

MeanStdDevNormalization CreateMeanStdDevNormalization(
    const OperationDef& definition);

}  // namespace cl
}  // namespace gpu
}  // namespace tflite

#endif  // TENSORFLOW_LITE_DELEGATES_GPU_CL_KERNELS_LSTM_NORMALIZATION_H_
