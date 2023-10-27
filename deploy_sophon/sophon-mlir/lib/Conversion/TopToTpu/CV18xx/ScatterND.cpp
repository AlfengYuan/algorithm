//===----------------------------------------------------------------------===//
//
// ScatterNDright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//

#include "tpu_mlir/Conversion/TopToTpu/LoweringCV18xx.h"

#define DEBUG_TYPE "lowering-ScatterND"
namespace tpu_mlir {
namespace cv18xx {
void ScatterNDLowering::LoweringINT8(PatternRewriter &rewriter, top::ScatterNDOp op,
                                   bool asymmetric) const {
  if (!module::isWeight(op.getIndices())) {
    llvm_unreachable("Not support activation indices.");
  }
  auto newType = getQuantInt8Type(op.getOutput(), asymmetric);
  std::vector<Value> operands;
  operands.push_back(op.getInputData());
  operands.push_back(op.getIndices());
  operands.push_back(op.getUpdates());
  auto noneOp = module::getNoneOp(op);
  operands.push_back(noneOp);
  rewriter.replaceOpWithNewOp<tpu::ScatterNDOp>(op, newType, operands, op->getAttrs());
}

void ScatterNDLowering::LoweringBF16(PatternRewriter &rewriter,
                                   top::ScatterNDOp op) const {
  if (!module::isWeight(op.getIndices())) {
    llvm_unreachable("Not support activation indices.");
  }
  auto newType = getQuantBF16Type(op.getOutput());
  std::vector<Value> operands;
  operands.push_back(op.getInputData());
  operands.push_back(op.getIndices());
  operands.push_back(op.getUpdates());
  auto noneOp = module::getNoneOp(op);
  operands.push_back(noneOp);
  rewriter.replaceOpWithNewOp<tpu::ScatterNDOp>(op, newType, operands, op->getAttrs());
}

} // namespace cv18xx
} // namespace tpu_mlir
