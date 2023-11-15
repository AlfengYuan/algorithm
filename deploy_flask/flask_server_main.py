# This is a sample Python script.
import os.path

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from flask import Flask, request
from PIL import Image
import io
import numpy as np
import pandas as pd
import face_recognition
import pdb
import gevent
from gevent import pywsgi
import json
import base64
app = Flask(__name__)
projects = {"face": ["cjjb"], "upload": ["sqsy"]}

FACE_ENCODE_URL = '/face-encode/<project>'
FACE_COMPARE_URL = '/face-compare/<project>'
UPLOAD_URL = '/upload/<project>'

@app.route(FACE_ENCODE_URL, methods=['POST'])
def face_encode(project):
    if request.method != 'POST':
        return {"error": "request method only support POST"}
    if request.json.get('image'):
        im_bytes = base64.b64decode(request.json['image'].encode("utf-8"))
        # im_bytes = im_file.read()
        im = np.array(Image.open(io.BytesIO(im_bytes)).convert('RGB'))
        if project in projects["face"]:
            # pdb.set_trace()
            features = face_recognition.face_encodings(face_image=im, known_face_locations=None, num_jitters=1,
                                                            model='large')
            if len(features) <= 0:
                return {"error": "not find face in image"}
            return ','.join([str(i) for i in list(features[0])])
        else:
            return {"error": "not support {}".format(project)}
    else:
        return {"error": "not right image data, please use json type"}

@app.route(FACE_COMPARE_URL, methods=['POST'])
def face_compare(project):
    if request.method != 'POST':
        return {"error": "request method only support POST"}
    if request.json.get('image') and request.json.get('features'):
        im_bytes = base64.b64decode(request.json['image'].encode("utf-8"))
        # im_bytes = im_file.read()
        im = np.array(Image.open(io.BytesIO(im_bytes)).convert('RGB'))
        list_of_face_encodings = list(map(lambda x: np.array(list(map(lambda y: float(y), x.split(',')))),
                                          request.json.get('features').split(';')))
        if project in projects["face"]:
            # pdb.set_trace()
            a_single_unknow_face_encoding = face_recognition.face_encodings(face_image=im, known_face_locations=None,
                                                                            num_jitters=1, model='large')
            if len(a_single_unknow_face_encoding) <= 0:
                return {"error": "not find face in image"}

            distances_index = str(face_recognition.face_distance(list_of_face_encodings,
                                                                 a_single_unknow_face_encoding[0]).argmin())

            return distances_index
        else:
            return {"error": "not support {}".format(project)}
    else:
        return {"error": "not right image data or features data, please use correct json type"}

@app.route("/my_fcn")
def my_fcn():
    return "hello, world"\

@app.route(UPLOAD_URL, methods=['POST'])
def upload(project):
    if request.method != 'POST':
        return {"error": "request method only support POST"}
    if request.files.get('image'):
        # Method 1
        # with request.files["image"] as f:
        #     im = Image.open(io.BytesIO(f.read()))

        # Method 2
        im_file = request.files['image']
        im_bytes = im_file.read()
        im = Image.open(io.BytesIO(im_bytes))

        if project in projects["upload"]:
            im.save("test.jpg")
            if os.path.exists("test.jpg"):
                return "SUCCESS"
            else:
                return {"error": "failed to save image"}
        else:
            return {"error": "not support {}".format(project)}
    else:
        return {"error": "not right image data, please use json type"}




def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    # app.run(host='0.0.0.0', port=6008)
    server = pywsgi.WSGIServer(('0.0.0.0', 6008), app)
    server.serve_forever()
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
