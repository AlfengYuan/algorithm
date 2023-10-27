//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//
#include "tpu_mlir/Conversion/TopToTpu/LoweringCV18xx.h"

#define DEBUG_TYPE "lowering-Sigmoid"
namespace tpu_mlir {
namespace cv18xx {
static double active_sigmoid(double val) { return 1 / (1 + std::exp(-val)); }
static double active_logsigmoid(double val) {
  return std::log(1 / (1 + std::exp(-val)));
}

void SigmoidLowering::LoweringINT8(PatternRewriter &rewriter, top::SigmoidOp op,
                                   bool asymmetric) const {
  Value table =
      create_lookup_table(op.getInput(), op.getOutput(), asymmetric,
                          op.getLog() ? active_logsigmoid : active_sigmoid);
  auto newType = getQuantInt8Type(op.getOutput(), asymmetric);
  rewriter.replaceOpWithNewOp<tpu::LutOp>(op, newType,
                                          ValueRange{op.getInput(), table});
}

void SigmoidLowering::LoweringBF16(PatternRewriter &rewriter,
                                   top::SigmoidOp op) const {
  Value table_weight, slope_weight;
  float range_start = -12, range_end = 12;
  if (op.getLog()) {
    range_start = -3;
    range_end = 3;
    createBf16LutOp(op, "slope", TableMode::Slope, 0.0, 0.0, range_start,
                    range_end, active_logsigmoid, table_weight, slope_weight);
  } else {
    range_start = -12;
    range_end = 12;
    createBf16LutOp(op, "slope", TableMode::Slope, 0.0, 0.0, range_start,
                    range_end, active_sigmoid, table_weight, slope_weight);
  }
  std::vector<NamedAttribute> attrs;
  for (auto &attr : op->getAttrs()) {
    attrs.emplace_back(attr);
  }
  attrs.push_back(rewriter.getNamedAttr(
      "lut_mode",
      tpu::LutBF16ModeAttr::get(op->getContext(), tpu::LutBF16Mode::Slope)));
  attrs.push_back(rewriter.getNamedAttr("min_range",
                                        rewriter.getF64FloatAttr(range_start)));
  attrs.push_back(
      rewriter.getNamedAttr("max_range", rewriter.getF64FloatAttr(range_end)));
  auto newType = getQuantBF16Type(op.getOutput());
  rewriter.replaceOpWithNewOp<tpu::LutBF16Op>(
      op, newType, ValueRange{op.getInput(), table_weight, slope_weight},
      attrs);
  return;
}
} // namespace cv18xx
} // namespace tpu_mlir
