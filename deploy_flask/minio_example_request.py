# -- coding: utf-8 --
# @Time: 2023/12/27
# @Author: yyf
# @File: minio_example_request.py

import base64
import requests
from PIL import Image
import io
import numpy as np
import paho.mqtt.subscribe as subscribe
import time
TOPIC = "MQTT Examples"
DOWNLOAD_URL = 'http://localhost:6009/minio-download/sqsy'

def get_obj_name(strmsg):
    strmsg = strmsg.strip('{').strip('}')
    result = ""
    for m in strmsg.split(","):
        result += "/" + m.split(':')[1].strip(' ')
    return result + ".jpg"

while True:
    msg = subscribe.simple(topics=TOPIC, qos=2, hostname='broker.emqx.io', port=1883)
    strmsg = msg.payload.decode("utf-8")
    object_name = get_obj_name(strmsg)
    bucket_name = "sqsy"
    response = requests.post(DOWNLOAD_URL, json={'bucket_name': bucket_name, 'object_name': object_name})
    img = base64.b64decode(response.text.encode("utf-8"))
    date = np.array(Image.open(io.BytesIO(img)).convert('RGB'))
    im = Image.fromarray(date)
    timesnap = str(int(time.time())) + ".jpg"
    im.save(timesnap)
    print("{} ---> {}".format(object_name, timesnap))