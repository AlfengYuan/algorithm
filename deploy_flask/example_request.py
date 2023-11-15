# YOLOv5 🚀 by Ultralytics, AGPL-3.0 license
"""
Perform test request
"""
import base64
import pprint

import requests

# DETECTION_URL = 'http://localhost:5000/v1/object-detection/yolov5s'
DETECTION_URL = 'http://localhost:6008/face-encode/cjjb'
# IMAGE = 'zidane.jpg'
# IMAGE = '20200908221516829.png'
# IMAGE = "/home/yyf/Documents/PycharmProjects/yolov5/utils/flask_rest_api/vlcsnap-2023-11-14-14h18m20s882.png"
IMAGE = "26ed6088dd203bbd7023a64fb263d251.png"
# Read image
with open(IMAGE, 'rb') as f:
    image_data = base64.b64encode(f.read()).decode("utf-8")

response = requests.post(DETECTION_URL, json={'image': image_data})
pprint.pprint(response.text)
