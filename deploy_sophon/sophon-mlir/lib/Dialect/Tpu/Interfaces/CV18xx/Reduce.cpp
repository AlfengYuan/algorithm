//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//

#include "tpu_mlir/Backend/CV18xx/CV18xx_global_api.h"
#include "tpu_mlir/Dialect/Tpu/IR/TpuOps.h"




using namespace tpu_mlir::backend;

// =========================================
// GlobalGenInterface
// =========================================
void tpu::ReduceOp::codegen_global_cv18xx(int64_t layer_id) {
  gaddr_t ga_input = module::getAddress(getInput());
  gaddr_t ga_output = module::getAddress(getOutput());
  std::vector<int32_t> axes_v;
  auto mode_ = getMode();
  auto axes_val = module::getI64Array(getAxes());
  axes_v.assign(axes_val->begin(), axes_val->end());
  std::vector<int64_t> input_shape = module::getShape(getInput());
  if (mode_ == "ReduceL2") {
    gaddr_t ga_table = module::getAddress(getBuffer());
    gaddr_t ga_mantissa_table = module::getAddress(getReciprocalMantissaTable());
    cvi_backend_tg_bf16_reduce_l2_kernel(layer_id, ga_input, ga_output,
                                         ga_table, ga_mantissa_table,
                                         input_shape, axes_v);
    return;
  }

  if (module::isUniformQuantized(getOutput())) {
    int32_t shift =
        static_cast<int32_t>(module::getI64Array(getRshift().value())->at(0));
    int32_t multi =
        static_cast<int32_t>(module::getI64Array(getMultiplier().value())->at(0));
    if (mode_ == "ReduceMean") {
      cvi_backend_tg_fixed_reduce_mean_kernel(
          layer_id, ga_input, ga_output, input_shape, axes_v, multi, shift);
    } else if (mode_ == "ReduceSum") {
      cvi_backend_tg_fixed_reduce_sum_kernel(layer_id, ga_input, ga_output,
                                             input_shape, axes_v, multi, shift);
    } else if (mode_ == "ReduceMax") {
      cvi_backend_tg_fixed_reduce_max_kernel(layer_id, ga_input, ga_output,
                                             input_shape, axes_v);
    } else if (mode_ == "ReduceMin") {
      cvi_backend_tg_fixed_reduce_min_kernel(layer_id, ga_input, ga_output,
                                             input_shape, axes_v, multi, shift);
    } else {
      llvm_unreachable("unsupport reduce type.");
    }

  } else {
    if (mode_ == "ReduceMean") {
      cvi_backend_tg_bf16_reduce_mean_kernel(layer_id, ga_input, ga_output,
                                             input_shape, axes_v);
    } else if (mode_ == "ReduceSum") {
      cvi_backend_tg_bf16_reduce_sum_kernel(layer_id, ga_input, ga_output,
                                            input_shape, axes_v);
    } else if (mode_ == "ReduceMax") {
      cvi_backend_tg_bf16_reduce_max_kernel(layer_id, ga_input, ga_output,
                                            input_shape, axes_v);
    } else if (mode_ == "ReduceMin") {
      cvi_backend_tg_bf16_reduce_min_kernel(layer_id, ga_input, ga_output,
                                            input_shape, axes_v);
    } else {
      llvm_unreachable("unsupport reduce type.");
    }
  }
}
