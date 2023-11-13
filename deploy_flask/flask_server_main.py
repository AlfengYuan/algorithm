# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from flask import Flask, request
from PIL import Image
import io
import numpy as np
import pandas as pd
import face_recognition
import pdb

app = Flask(__name__)
projects = {"cjjb"}

FACE_ENCODE_URL = '/face-encode/<project>'

@app.route(FACE_ENCODE_URL, methods=['POST'])
def face_encode(project):
    if request.method != 'POST':
        return

    result = {}
    if request.files.get('image'):
        im_file = request.files['image']
        im_bytes = im_file.read()
        im = np.array(Image.open(io.BytesIO(im_bytes)).convert('RGB'))

        print(type(im))

        if project in projects:
            # pdb.set_trace()
            features = list(face_recognition.face_encodings(im)[0])
            result["feature"] = features

    return result


@app.route("/my_fcn")
def my_fcn():
    return "hello, world"\

def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6008)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
