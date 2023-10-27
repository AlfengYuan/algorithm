#!/usr/bin/env python3
# Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
#
# TPU-MLIR is licensed under the 2-Clause BSD License except for the
# third-party components.
#
# ==============================================================================

import numpy as np
import os, sys
import transform.TpuLang as tpul
from typing import List


def rand_data(shape, dtype):
    if dtype == 'float32':
        return np.random.random(shape).astype(np.float32)
    if dtype == 'int32' or 'uint32' or 'int16' or 'uint16' or 'int8' or 'uint8':
        return np.random.randint(0, 256, size=shape).astype(dtype)
    raise Exception("Not supported data type: {}!".format(dtype))


def tpulang(func):

    def wrapper(*args, **kwargs):
        tpul.init("BM1684X", True)
        func(*args, **kwargs)
        tpul.deinit()

    return wrapper


Failed_Cases = []


class TPULANG_IR_TESTER(object):
    ID = 0

    # This class is built for testing single operator transform.
    def __init__(self, chip: str = "bm1684x"):
        Y, N = True, False
        self.test_function = {
            #############################
            # TpuLang Test Case, Alphabetically
            #############################
            # case:                 (test,          bm1684x_support)
            "Add": (self.test_Add, Y),
            "Conv2d": (self.test_Conv2d, Y),
            "HModel": (self.test_Model, N),
            "Mul": (self.test_Mul, Y),
            "Sub": (self.test_Sub, Y),
            "Custom": (self.test_Custom, Y),
            "MatMul": (self.test_MatMul, Y),
            "Maxpool": (self.test_Maxpool, Y),
            "Relu": (self.test_Relu, Y),
            "Lenet": (self.test_Lenet, Y)
        }
        # no quantization when quant_mode == "f32"
        self.quant_modes = ["int8"]
        self.chip = chip.lower()

    def test_single(self, case: str):
        TPULANG_IR_TESTER.ID = 0
        print("Test: {}".format(case))
        if case in self.test_function:
            os.makedirs(case, exist_ok=True)
            os.chdir(case)
            func, _ = self.test_function[case]
            func(case)
            print("====== TEST {} Success ======".format(case))
        else:
            raise RuntimeError("case [{}] is not exist".format(case))

    def check_support(self, case):
        _, bm1684x_support = self.test_function[case]
        if self.chip == "bm1684x" and bm1684x_support:
            return True
        return False

    def test_all(self):
        for case in self.test_function:
            if case not in Failed_Cases:
                self.test_single(case)
        print("====== ALL TEST Success ======".format(case))

    def list(self):
        print("====== All Support Ops ======")
        for case in self.test_function:
            if case not in Failed_Cases:
                print(case)
        print("====== Error Ops ======")
        for case in self.test_function:
            if case in Failed_Cases:
                print(case)

    def coeff_tensor(self, shape, dtype, scale=1.0):
        data = rand_data(shape, dtype)
        data = data * scale if dtype == 'float32' else data
        return tpul.Tensor(dtype=dtype, shape=shape, data=data, is_const=True)

    #######################################################################
    # Add
    # ------------
    def add_op(self, input_0, input_1, dtype="float32"):
        out_dtype = dtype if dtype == 'float32' else 'int32'
        add = tpul.add(input_0, input_1, out_dtype)
        return add

    def test_Add(self, case_name):
        """Add"""

        @tpulang
        def _test_add(shape_x: List[int], shape_y: List[int], dtype="float32"):
            x_data = rand_data(shape_x, dtype)
            y_data = rand_data(shape_y, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=x_data)
            y = tpul.Tensor(dtype=dtype, shape=shape_y, data=y_data)
            add = self.add_op(x, y, dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [add], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_add([1, 3, 28, 28], [1, 3, 28, 28])
        _test_add([1, 3, 32, 32], [1, 3, 32, 32])
        _test_add([1, 3, 32, 32], [1, 1, 32, 32])
        _test_add([1, 3, 32, 32], [1])
        _test_add([1], [1, 3, 32, 32])
        _test_add(
            [1, 1, 32, 32],
            [1, 3, 1, 32],
            dtype="int8",
        )

    #######################################################################
    # Convolution
    # ------------
    def conv_op(self,
                x,
                kshape,
                stride,
                pad=None,
                group=1,
                dilation=[1, 1],
                zp=[None, None],
                dtype="float32"):
        oc = kshape[0]
        weight = self.coeff_tensor(kshape, dtype)
        out_dtype = dtype if dtype == 'float32' else 'int32'
        bias = self.coeff_tensor(oc, out_dtype)
        conv = tpul.conv_v2(x,
                            weight,
                            bias=bias,
                            stride=stride,
                            pad=pad,
                            dilation=dilation,
                            group=group,
                            input_zp=zp[0],
                            weight_zp=zp[1],
                            out_dtype=out_dtype)
        return conv

    def test_Conv2d(self, case_name):
        """Conv 2D"""

        @tpulang
        def _test_convolution(input_shape: List[int],
                              kernel_shape: List[int],
                              stride: List[int] = [1, 1],
                              dilation: List[int] = [1, 1],
                              pad: List[int] = None,
                              group=1,
                              dtype="float32",
                              zp: List[int] = [None, None]):
            x_data = rand_data(input_shape, dtype)
            x = tpul.Tensor(dtype=dtype, shape=input_shape, data=x_data)
            conv = self.conv_op(x,
                                kernel_shape,
                                stride,
                                pad,
                                group=group,
                                dilation=dilation,
                                zp=zp,
                                dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [conv], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_convolution([1, 3, 28, 28], [12, 3, 1, 1], group=3)
        _test_convolution([1, 3, 32, 32], [12, 3, 3, 3], stride=[2, 2], pad=[1, 1, 1, 1])
        _test_convolution([1, 3, 32, 32], [12, 3, 3, 3],
                          stride=[2, 2],
                          pad=[1, 1, 1, 1],
                          dtype="int8",
                          zp=[5, -8])

    #######################################################################
    # HModel
    # ------------
    def test_Model(self, case_name):

        def model_def(x):
            rq0 = tpul.requant_fp_to_int(x, 1.0, 0, 0, 'int8')
            conv1 = self.conv_op(rq0, [64, 1, 7, 7], [2, 2], None, zp=[0, 0], dtype='int8')
            # rq2 = tpul.requant_int(conv1, 2030043136, -13, 0, 0, 'int8', round_mode='half_away_from_zero')
            # relu3 = tpul.relu(rq2)
            # conv4 = conv_op(relu3, [96,64,3,3], [2,2], None, zp=[0,0], dtype='int8')
            # rq5 = tpul.requant_int(conv4, 1748893696, -10, 0, 0, 'int8', round_mode='half_away_from_zero')
            # relu6 = tpul.relu(rq5)
            # dq7 = tpul.dequant_int_to_fp(relu6, 0.25, 0)
            # coeff8 = coeff_tensor([1,96,1,1], 'float32', 10.0)
            # tpul.constdata(coeff8)
            # mul9 = tpul.mul(dq7, coeff8)
            # coeff10 = coeff_tensor([1,96,1,1], 'float32', -2.0)
            # tpul.constdata(coeff10)
            # add11 = tpul.add(mul9, coeff10)
            # relu12 = tpul.relu(add11)
            # rq13 = tpul.requant_fp_to_int(relu12, 4.0, 0, 0, 'int8')
            # conv14 = conv_op(rq13, [96,96,3,3], [1,1], [1,1,1,1], zp=[0,0], dtype='int8')
            # rq15 = tpul.requant_int(conv14, 1623457792, -8, 0, 0, 'int8', round_mode='half_away_from_zero')
            # relu16 = tpul.relu(rq15)
            # conv17 = conv_op(relu16, [96,96,3,3], [1,1], [1,1,1,1], zp=[0,0], dtype='int8')
            # rq18 = tpul.requant_int(conv17, 1623457792, -10, 0, 0, 'int8', round_mode='half_away_from_zero')
            # dq19 = tpul.dequant_int_to_fp(rq18, 0.0625, 0)
            # add20 = tpul.add(dq19, dq7)
            # coeff21 = coeff_tensor([1,96,1,1], 'float32', 2.0)
            # tpul.constdata(coeff21)
            # mul22 = tpul.mul(add20, coeff21)
            # coeff23 = coeff_tensor([1,96,1,1], 'float32', -2.0)
            # tpul.constdata(coeff23)
            # add24 = tpul.add(mul22, coeff23)
            # relu25 = tpul.relu(add24)
            # rq26 = tpul.requant_fp_to_int(relu25, 8.0, 0, 0, 'int8')
            # conv27 = conv_op(rq26, [96,96,3,3], [1,1], [1,1,1,1], zp=[0,0], dtype='int8')
            # rq28 = tpul.requant_int(conv27, 1712717824, -7, 0, 0, 'int8', round_mode='half_away_from_zero')
            # dq29 = tpul.dequant_int_to_fp(rq28, 0.0625, 0)
            return conv1

        @tpulang
        def _test_model_def(in_shape):
            x_data = (rand_data(in_shape, 'float32') - 0.5) * 256
            x = tpul.Tensor(dtype='float32', shape=in_shape, data=x_data)
            out = model_def(x=x)
            tpul.compile(case_name, [x], [out], False, 2)

        _test_model_def([1, 3, 28, 28])

    #######################################################################
    # Convolution
    # ------------
    def conv_op(self,
                x,
                kshape,
                stride,
                pad=None,
                group=1,
                dilation=[1, 1],
                zp=[None, None],
                dtype="float32"):
        oc = kshape[0]
        weight = self.coeff_tensor(kshape, dtype)
        out_dtype = dtype if dtype == 'float32' else 'int32'
        bias = self.coeff_tensor(oc, out_dtype)
        conv = tpul.conv_v2(x,
                            weight,
                            bias=bias,
                            stride=stride,
                            pad=pad,
                            dilation=dilation,
                            group=group,
                            input_zp=zp[0],
                            weight_zp=zp[1],
                            out_dtype=out_dtype)
        return conv

    def test_Conv2d(self, case_name):
        """Conv 2D"""

        @tpulang
        def _test_convolution(input_shape: List[int],
                              kernel_shape: List[int],
                              stride: List[int] = [1, 1],
                              dilation: List[int] = [1, 1],
                              pad: List[int] = None,
                              group=1,
                              dtype="float32",
                              zp: List[int] = [None, None]):
            x_data = rand_data(input_shape, dtype)
            x = tpul.Tensor(dtype=dtype, shape=input_shape, data=x_data)
            conv = self.conv_op(x,
                                kernel_shape,
                                stride,
                                pad,
                                group=group,
                                dilation=dilation,
                                zp=zp,
                                dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [conv], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_convolution([1, 3, 28, 28], [12, 3, 1, 1], group=3)
        _test_convolution([1, 3, 32, 32], [12, 3, 3, 3], stride=[2, 2], pad=[1, 1, 1, 1])
        _test_convolution([1, 3, 32, 32], [12, 3, 3, 3],
                          stride=[2, 2],
                          pad=[1, 1, 1, 1],
                          dtype="int8",
                          zp=[5, -8])

    #######################################################################
    # Lenet
    # ------------
    def test_Lenet(self, case_name):

        def model_lenet(x):
            conv0 = self.conv_op(x, kshape=[32, 1, 5, 5], stride=[1,1], pad=[2, 2, 2, 2], dtype='float32')
            relu1 = self.relu_op(conv0)
            maxpool2 = self.maxpool_op(relu1, kshape=[2, 2], stride=[2, 2], pad=[0, 0, 0, 0])
            conv3 = self.conv_op(maxpool2, kshape=[64, 32, 5, 5], stride=[1,1], pad=[2, 2, 2, 2], dtype='float32')
            relu4 =  self.relu_op(conv3)
            maxpool5 =self.maxpool_op(relu4, kshape=[2, 2], stride=[2, 2], pad=[0, 0, 0, 0])
            conv6 = self.conv_op(maxpool5, kshape=[1024, 64, 7, 7], stride=[1,1], dtype='float32')
            relu7 =  self.relu_op(conv6)
            conv9 = self.conv_op(relu7,  kshape=[10, 1024, 1, 1], stride=[1,1], dtype='float32')
            return conv9

        @tpulang
        def _test_lenet(in_shape):
            x_data = (rand_data(in_shape, 'float32') - 0.5) * 256
            x = tpul.Tensor(dtype='float32', shape=in_shape, data=x_data)
            out = model_lenet(x=x)
            tpul.compile(case_name, [x], [out], False, 2)

        _test_lenet([1, 1, 28, 28])

    #######################################################################
    # Mul
    # ------------
    def mul_op(self, input_0, input_1, dtype="float32"):
        out_dtype = dtype if dtype == 'float32' else 'int32'
        mul = tpul.mul(input_0, input_1, out_dtype)
        return mul

    def test_Mul(self, case_name):
        """Mul"""

        @tpulang
        def _test_mul(shape_x: List[int], shape_y: List[int], dtype="float32"):
            x_data = rand_data(shape_x, dtype)
            y_data = rand_data(shape_y, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=x_data)
            y = tpul.Tensor(dtype=dtype, shape=shape_y, data=y_data)
            mul = self.mul_op(x, y, dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [mul], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_mul([1, 3, 28, 28], [1, 3, 28, 28])
        _test_mul([1, 3, 32, 32], [1, 3, 32, 32])
        _test_mul([1, 3, 32, 32], [1, 1, 32, 32])
        _test_mul([1, 3, 32, 32], [1])
        _test_mul([1], [1, 3, 32, 32])
        _test_mul(
            [1, 1, 32, 32],
            [1, 3, 1, 32],
            dtype="int8",
        )

    #######################################################################
    # Sub
    # ------------
    def sub_op(self, input_0, input_1, dtype="float32"):
        out_dtype = dtype if dtype == 'float32' else 'int32'
        sub = tpul.sub(input_0, input_1, out_dtype)
        return sub

    def test_Sub(self, case_name):
        """Sub"""

        @tpulang
        def _test_sub(shape_x: List[int], shape_y: List[int], dtype="float32"):
            x_data = rand_data(shape_x, dtype)
            y_data = rand_data(shape_y, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=x_data)
            y = tpul.Tensor(dtype=dtype, shape=shape_y, data=y_data)
            sub = self.sub_op(x, y, dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [sub], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_sub([1, 3, 28, 28], [1, 3, 28, 28])
        _test_sub([1, 3, 32, 32], [1, 3, 32, 32])
        _test_sub([1, 3, 32, 32], [1, 1, 32, 32])
        _test_sub([1, 3, 32, 32], [1])
        _test_sub([1], [1, 3, 32, 32])
        _test_sub(
            [1, 1, 32, 32],
            [1, 3, 1, 32],
            dtype="int8",
        )

    #######################################################################
    # Custom
    # ------------
    def custom_op(self, inputs, shape_func, op_name, params, dtypes, out_names=None):
        custom = tpul.custom(inputs,
                             shape_func,
                             op_name,
                             out_dtypes=dtypes,
                             params=params,
                             out_names=out_names)
        return custom

    def test_Custom(self, case_name):
        """Custom test sample"""

        @tpulang
        def _test_base(op_name: str, input_shapes: list, params: dict, shape_func, infer_func,
                       out_names, dtype: str):
            input_data = []
            inputs = []
            for input_shape in input_shapes:
                x_data = rand_data(input_shape, dtype)
                input_data.append(x_data)
                x = tpul.Tensor(dtype=dtype, shape=input_shape, data=x_data)
                inputs.append(x)

            outs = self.custom_op(inputs,
                                  shape_func,
                                  op_name,
                                  params=params,
                                  dtypes=[dtype],
                                  out_names=out_names)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID),
                         inputs,
                         outs,
                         False,
                         2,
                         has_custom=True)
            TPULANG_IR_TESTER.ID += 1
            # save the origin output for comparison
            # There are two outputs because in non-f32 quant mode, the result will be
            # dequant back to f32 with castOp so that the final result will be named with the suffix '_f32'
            if infer_func:
                origin_out = infer_func(input_data)
                out = {}
                for out_name in out_names:
                    out[out_name] = origin_out
                    out[f"{out_name}_f32"] = origin_out
                np.savez(f"{op_name}_target_data", **out)

        def _test_swap_channel(input_shapes: list, dtype="float32"):

            def shape_func(tensors_in):
                return [tensors_in[0].shape]

            def infer_func(np_data):
                return [np_data[0][:, [2, 1, 0], :, :]]

            params = {"order": [2, 1, 0]}
            _test_base("swapchannel", input_shapes, params, shape_func, infer_func, ["out"], dtype)

        def _test_abs_add(input_shapes: list, dtype="float32"):

            def shape_func(tensors_in):
                return [tensors_in[0].shape]

            def infer_func(np_data):
                return [np.abs(np_data[0]) + 1.2]

            params = {"b_val": 1.2}
            _test_base("absadd", input_shapes, params, shape_func, infer_func, ["out"], dtype)

        def _test_ceil_add(input_shapes: list, dtype="float32"):

            def shape_func(tensors_in):
                return [tensors_in[0].shape]

            def infer_func(np_data):
                return [np.ceil(np_data[0]) + 1.5]

            params = {"b_val": 1.5}
            _test_base("ceiladd", input_shapes, params, shape_func, infer_func, ["out"], dtype)

        _test_swap_channel([[1, 3, 14, 14]])
        _test_abs_add([[1, 3, 14, 14]])
        _test_ceil_add([[1, 3, 14, 14]])

    #######################################################################
    # Matmul
    # ------------
    def matmul_op(self, left, right, dtype="float32"):
        matmul = tpul.matmul(left, right)
        return matmul

    def test_MatMul(self, case_name):
        """Matmul"""

        @tpulang
        def _test_matmul(shape_x: List[int], shape_y: List[int], dtype="float32"):
            left = rand_data(shape_x, dtype)
            right = rand_data(shape_y, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=left)
            y = tpul.Tensor(dtype=dtype, shape=shape_y, data=right)
            matmul = self.matmul_op(x, y, dtype=dtype)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [matmul], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_matmul([1, 3, 28, 10], [1, 3, 10, 8])

    #######################################################################
    # Maxpool
    # ------------
    def maxpool_op(self,
                input_0,
                kshape,
                stride,
                pad=None,
                ceil_mode=False):
        maxpool = tpul.maxpool(input_0, kshape, stride, pad, ceil_mode)
        return maxpool

    def test_Maxpool(self, case_name):
        """Maxpool"""

        @tpulang
        def _test_maxpool(shape_x: List[int],
                                kshape: List[int] = [1,1],
                                stride: List[int] = [1, 1],
                                pad: List[int] = None,
                                dtype="float32"):
            input = rand_data(shape_x, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=input)
            maxpool = self.maxpool_op(x, kshape, stride, pad)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [maxpool], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_maxpool([1, 32, 28, 28], kshape = [2, 2], stride = [2, 2], pad=[0, 0, 0, 0])

    #######################################################################
    # Relu
    # ------------
    def relu_op(self, input_0):
        relu = tpul.relu(input_0)
        return relu

    def test_Relu(self, case_name):
        """Relu"""

        @tpulang
        def _test_relu(shape_x: List[int], dtype="float32"):
            input = rand_data(shape_x, dtype)
            x = tpul.Tensor(dtype=dtype, shape=shape_x, data=input)
            relu = self.relu_op(x)
            tpul.compile("{}_{}".format(case_name, TPULANG_IR_TESTER.ID), [x], [relu], False, 2)
            TPULANG_IR_TESTER.ID += 1

        _test_relu([1, 32, 28, 28])

def test_one_case_in_all(tester: TPULANG_IR_TESTER, case, error_cases, success_cases):
    import traceback
    try:
        tester.test_single(case)
    except:
        error_cases.append(case)
        traceback.print_exc()
        return
    success_cases.append(case)


def test_all(tester: TPULANG_IR_TESTER):
    import multiprocessing
    from utils.misc import collect_process
    process_number = multiprocessing.cpu_count() // 2 + 1
    processes = []
    error_cases = multiprocessing.Manager().list()
    success_cases = multiprocessing.Manager().list()
    for case in tester.test_function:
        if tester.check_support(case):
            p = multiprocessing.Process(target=test_one_case_in_all,
                                        name=case,
                                        args=(tester, case, error_cases, success_cases))
            processes.append(p)
        if len(processes) == process_number:
            collect_process(processes, error_cases)
            processes = []
    collect_process(processes, error_cases)
    processes = []
    # error_cases = []
    # success_cases = []
    # for case in tester.test_cases:
    #     if tester.check_support(case):
    #         test_one_case_in_all(tester, case, error_cases, success_cases)
    print("Success: {}".format(success_cases))
    print("Failure: {}".format(error_cases))
    if error_cases:
        print("====== test_tpulang.py --chip {} TEST Failed ======".format(tester.chip))
        # exit(1)
    else:
        print("====== test_tpulang.py --chip {} TEST Success ======".format(tester.chip))
    return error_cases


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    # yapf: disable
    parser.add_argument("--chip", default="bm1684x", type=str,
                        choices=['bm1684x'], help="chip platform name")
    parser.add_argument("--case", default="all", type=str, help="test one case, if all, then test all cases")
    parser.add_argument("--show_all", action="store_true", help='show all cases')
    # yapf: enable
    args = parser.parse_args()
    tester = TPULANG_IR_TESTER(args.chip)
    if args.show_all:
        print("====== Show All Cases ============")
        for case in tester.test_function:
            print(case)
        exit(0)
    dir = "tpulang_test_{}".format(args.chip)
    os.makedirs(dir, exist_ok=True)
    os.chdir(dir)
    if args.case == "" or args.case.lower() == "all":
        test_all(tester)
    else:
        tester.test_single(args.case)
