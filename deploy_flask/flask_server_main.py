# -- coding: utf-8 --
# @Time: 2023/12/27
# @Author: yyf
# @File: flask_server_main.py

# This is a sample Python script.
import os.path

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from flask import Flask, request
from PIL import Image
import io
import numpy as np
import face_recognition
from gevent import pywsgi
import base64
from ultralytics import YOLO
from server_configs import *
model = YOLO("yolov8n.pt")

app = Flask(__name__)

@app.route(FACE_ENCODE_URL, methods=['POST'])
def face_encode(project):
    if request.remote_addr not in HOSTS:
        return
    if request.method != 'POST':
        return {"error": "request method only support POST"}
    if request.json.get('image'):
        im_bytes = base64.b64decode(request.json['image'].encode("utf-8"))
        im = np.array(Image.open(io.BytesIO(im_bytes)).convert('RGB'))
        if project in PROJECTS["face"]:
            # pdb.set_trace()
            # features = face_recognition.face_encodings(face_image=im, known_face_locations=None, num_jitters=5,
            #                                                 model='large')
            if im is None:
                print("image data error!")
                return {"error": "image data error!"}

            # print("im data shape: {}".format(im.shape))
            # print("im data type: {}".format(type(im)))

            location = model.predict(im, conf=0.5)[0].boxes.data.cpu().numpy()
            location = location[location[:, 5] == 0]
            # if len(features) <= 0:
            #     return {"error": "not find face in image"}
            if location.shape[0] == 0:
                print("not find face in image")
                return {"error": "not find face in image"}
            tr_y, tr_x, lb_y, lb_x = int(location[0][1]), int(location[0][2]), int(location[0][3]), int(location[0][0])
            features = face_recognition.face_encodings(face_image=im, known_face_locations=[(tr_y, tr_x, lb_y, lb_x)],
                                                       num_jitters=1, model='large')
            return ','.join([str(i) for i in list(features[0])])
        else:
            print("not support {}".format(project))
            return {"error": "not support {}".format(project)}
    else:
        print("not right image data, please use json type")
        return {"error": "not right image data, please use json type"}


@app.route(FACE_COMPARE_URL, methods=['POST'])
def face_compare(project):
    if request.remote_addr not in HOSTS:
        return

    if request.method != 'POST':
        return {"error": "request method only support POST"}

    if request.json.get('image') and request.json.get('features'):
        im_bytes = base64.b64decode(request.json['image'].encode("utf-8"))
        # im_bytes = im_file.read()
        im = np.array(Image.open(io.BytesIO(im_bytes)).convert('RGB'))
        # list_of_face_encodings = list(map(lambda x: np.array(list(map(lambda y: float(y), x.split(',')))),
        #                                   request.json.get('features').split(';')))
        features = request.json.get('features')
        list_of_face_index = []
        list_of_face_encodings = []
        # pdb.set_trace()
        for i in features:
            id_index = int(i.get('id'))
            # ValueError: could not convert string to float: ''
            current_list_of_face_encodings = []
            for feature in i.get('feature').strip(' ').split(';'):
                if feature.strip(' ') != '':
                    current_feature = []
                    for f in feature.split(',')[:128]:
                        current_feature.append(float(f.strip(' ').strip('\r').strip('\n')))
                    current_list_of_face_encodings.append(current_feature)
                else:
                    continue
            list_of_face_index += [id_index] * len(current_list_of_face_encodings)
            list_of_face_encodings += current_list_of_face_encodings

        assert len(list_of_face_index) == len(list_of_face_encodings)
        if len(list_of_face_encodings) == 0:
            return "-1"
        if project in PROJECTS["face"]:
            # pdb.set_trace()
            # a_single_unknow_face_encoding = face_recognition.face_encodings(face_image=im, known_face_locations=None,
            #                                                                 num_jitters=5, model='large')
            locations = model.predict(im, conf=0.5)[0].boxes.data.cpu().numpy()
            locations = locations[locations[:, 5] == 0]
            # if len(features) <= 0:
            #     return {"error": "not find face in image"}
            if locations.shape[0] == 0:
                return {"error": "not find face in image"}
            if im is None:
                return {"error": "image data error!"}

            # print("im data shape: {}".format(im.shape))
            # print("im data type: {}".format(type(im)))

            for i in range(locations.shape[0]):
                tr_y, tr_x, lb_y, lb_x = (int(locations[i][1]), int(locations[i][2]), int(locations[i][3]),
                                          int(locations[i][0]))
                a_single_unknow_face_encoding = face_recognition.face_encodings(face_image=im,
                                                                                known_face_locations=[
                                                                                    (tr_y, tr_x, lb_y, lb_x)],
                                                                                num_jitters=1,
                                                                                model='large')
                distances = face_recognition.face_distance(list_of_face_encodings, a_single_unknow_face_encoding[0])
                index = distances.argmin()
                if distances[index] < 0.3:
                    print("success: {}".format(str(list_of_face_index[index])))
                    return str(list_of_face_index[index])
            return "-1"
        else:
            return {"error": "not support {}".format(project)}
    else:
        print("not right image data or features data, please use correct json type")
        return {"error": "not right image data or features data, please use correct json type"}


@app.route("/my_fcn")
def my_fcn():
    if request.remote_addr not in HOSTS:
        return
    return "hello, world"


@app.route(UPLOAD_URL, methods=['POST'])
def upload(project):
    if request.remote_addr not in HOSTS:
        return

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

        if project in PROJECTS["upload"]:
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
