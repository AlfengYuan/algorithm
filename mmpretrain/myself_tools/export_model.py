import torch
import torch.onnx
from torchvision.models import resnet50
from mmpretrain import get_model

# from collections import OrderedDict
#
# mmlab_model = torch.load("../work_dirs/resnet50_8xb32_in1k/epoch_100.pth", map_location=torch.device('cpu'))
# print(mmlab_model['state_dict'])

# mmlabkeys = list(mmlab_model['state_dict'].keys())
# print(mmlabkeys)


def torchvision_resnet50_2_onnx():
    input = torch.randn(1, 3, 224, 224)
    model = resnet50(weights='ResNet50_Weights.IMAGENET1K_V1')
    model.eval()
    torch.onnx.export(model, input, "torchvision_resnet50.onnx")

def get_mmpretrain_resnet50_onnx():
    input = torch.randn(1, 3, 224, 224)
    model = get_model("resnet50_8xb32_in1k", pretrained="../work_dirs/resnet50_8xb32_in1k/epoch_100.pth")
    model.eval()
    torch.onnx.export(model, input, "mmpretrain_resnet50.onnx")

def get_mmpretrain_resnet50_fenshui_onnx():
    input = torch.randn(1, 3, 224, 224)
    model = get_model("work_dirs/resnet50_8xb32_fenshu/resnet50_8xb32_fenshu.py", pretrained="work_dirs/resnet50_8xb32_fenshu/epoch_100.pth")
    model.eval()
    torch.onnx.export(model, input, "mmpretrain_resnet50_fenshui.onnx")

if __name__ == '__main__':
    # torchvision_resnet50_2_onnx()
    # get_mmpretrain_resnet50_model()
    get_mmpretrain_resnet50_fenshui_onnx()
    print("return 0")