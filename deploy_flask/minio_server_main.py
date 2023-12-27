# -- coding: utf-8 --
# @Time: 2023/12/27
# @Author: yyf
# @File: minio_server_main.py


import base64
import numpy as np
from PIL import Image
import io
from flask import Flask, request
from gevent import pywsgi
from server_configs import *
from minio import Minio


minioClient = Minio(endpoint='play.min.io', access_key='test', secret_key='test')
app = Flask(__name__)

@app.route(MINIO_OBJECT_DOWNLOAD_URL, methods=['POST'])
def minio_object_download(project):
    if request.remote_addr not in HOSTS:
        return
    if request.method != 'POST':
        return {"error": "request method only support POST"}

    if project not in PROJECTS["download"]:
        return {"error": "project is not right!"}

    if request.json.get('bucket_name'):
        bucket_name = request.json['bucket_name']
    else:
        return {"error": "not right bucket_name params by json type!"}

    if request.json.get('object_name'):
        object_name = request.json['object_name']
    else:
        return {"error": "not right object_name params by json type!"}



    result = base64.b64encode(minioClient.get_object(bucket_name, object_name).read()).decode("utf-8")

    return result

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    # app.run(host='0.0.0.0', port=6008)
    server = pywsgi.WSGIServer(('0.0.0.0', 6009), app)
    server.serve_forever()