import torch
from models.experimental import attempt_load
modelPath = 'runs/train/voc0712/weights/best.pt'
model = attempt_load(modelPath, map_location=torch.device('cpu'))
m = model.module.model[-1] if hasattr(model, 'module') else model.model[-1]
print(f'小目标anchor : {m.anchors[0]*m.stride[0]}')
print(f'中目标anchor : {m.anchors[1]*m.stride[1]}')
print(f'大目标anchor : {m.anchors[2]*m.stride[2]}')