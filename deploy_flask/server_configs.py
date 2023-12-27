# -- coding: utf-8 --
# @Time: 2023/12/27
# @Author: yyf
# @File: server_configs.py

PROJECTS = {"face": ["cjjb"], "upload": ["sqsy"], "download": ["sqsy"]}
HOSTS = ["127.0.0.1"]
FACE_ENCODE_URL = '/face-encode/<project>'
FACE_COMPARE_URL = '/face-compare/<project>'
UPLOAD_URL = '/upload/<project>'
MINIO_OBJECT_DOWNLOAD_URL = '/minio-download/<project>'