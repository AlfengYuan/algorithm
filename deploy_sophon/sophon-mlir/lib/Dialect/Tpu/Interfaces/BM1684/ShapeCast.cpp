//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//

#include "tpu_mlir/Dialect/Tpu/Transforms/Codegen/Dynamic/DynamicLayer.hpp"

using namespace tpu_mlir::backend;

void tpu::ShapeCastOp::codegen_global_bm1684() {
  llvm_unreachable("Not supported now");
}

uint32_t tpu::ShapeCastOp::dyn_codegen_global_bm1684(void* ir_layer_info) {
  int fw_ir_length = 0;
  ir_layer_info_t *add_layer_info = (ir_layer_info_t *)ir_layer_info;
  fw_shape_cast_layer_param_t fw_shape_cast_layer_param = {0};
  // TODO: fix it
  fw_shape_cast_layer_param.dst_type = 0;
  fw_shape_cast_layer_param.output_is_shape = 1;
  dynamic_common_ir_layer_info(add_layer_info, getInput(), getOutput());
  add_layer_info->fw_layer_param_u.fw_shape_cast_layer_param =
      fw_shape_cast_layer_param;
  fw_ir_length += sizeof(fw_shape_cast_layer_param_t);
  return fw_ir_length;
}

int64_t tpu::ShapeCastOp::get_fw_type_bm1684() {
  return FW_BMNET_SHAPE_CAST;
}
