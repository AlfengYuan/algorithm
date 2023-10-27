//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//

#include "tpu_mlir/Support/GenericCpuFunc.h"
#include "tpu_mlir/Support/MathUtils.h"

int64_t top::MishOp::getFLOPs() {
  return module::getNumElements(getOutput()) * 4;
}

LogicalResult top::MishOp::init(InferenceParameter &p) { return success(); }
void top::MishOp::deinit(InferenceParameter &p) {}

LogicalResult top::MishOp::inference(InferenceParameter &p) {
  auto num_element = module::getNumElements(getInput());
#pragma omp parallel for schedule(static, omp_schedule(num_element))
  for (int i = 0; i < num_element; ++i) {
    auto val = p.inputs[0][i];
    p.outputs[0][i] = my_mish_activate(val);
  }
  return success();
}

void top::MishOp::shape_inference() { common_shape_inference(getOperation()); }
