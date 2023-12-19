from minio import Minio
import base64
import numpy as np
from PIL import Image
import io
from flask import Flask, request
from gevent import pywsgi
minioClient = Minio(endpoint='paly.min.io', access_key='test', secret_key='test')
app = Flask(__name__)

@app.route('/minio_fcn')
def test():
    bucket_name = "my-bucket"
    object_name = "aa/bb/cc/20231115112043liuyu.jpg"

    result = base64.b64encode(minioClient.get_object(bucket_name, object_name).read()).decode("utf-8")

    return result

# img = base64.b64decode(result.encode("utf-8"))


# date = np.array(Image.open(io.BytesIO(img)).convert('RGB'))

# print(date.shape)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    # app.run(host='0.0.0.0', port=6008)
    server = pywsgi.WSGIServer(('0.0.0.0', 6009), app)
    server.serve_forever()