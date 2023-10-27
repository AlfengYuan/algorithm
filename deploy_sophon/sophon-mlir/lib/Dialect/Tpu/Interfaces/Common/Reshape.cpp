//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//


#include "tpu_mlir/Support/MathUtils.h"

LogicalResult tpu::ReshapeOp::init(InferenceParameter &p) { return success(); }
void tpu::ReshapeOp::deinit(InferenceParameter &p) {}

LogicalResult tpu::ReshapeOp::inference(InferenceParameter &p) {
  if (p.inputs[0] != p.outputs[0]) {
    auto num_elem = module::getNumElements(getOutput());
    memcpy(p.outputs[0], p.inputs[0], sizeof(float) * num_elem);
  }
  return success();
}

LogicalResult tpu::ReshapeOp::LocalGenSupport() {
  if (module::isCV18xx() || module::isBM1684Family()) {
    return failure();
  }

  auto ishape = module::getShape(getInput());
  auto oshape = module::getShape(getOutput());
  if (ishape.size() < 2 || oshape.size() < 2 || ishape[0] != oshape[0] ||
      ishape[1] != oshape[1]) {
    return failure();
  }
  return success();
}

LogicalResult tpu::ReshapeOp::AllowDataSplit(int64_t axis,
                                             group_type_t group_type) {
  if (axis == 0 || axis == 1) {
    return success();
  }

  return failure();
}
