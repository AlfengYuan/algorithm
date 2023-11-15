# YOLOv5 🚀 by Ultralytics, AGPL-3.0 license
"""
Perform test request
"""
import base64
import pprint

import requests

# DETECTION_URL = 'http://localhost:5000/v1/object-detection/yolov5s'
# DETECTION_URL = 'http://localhost:6008/face-encode/cjjb'
DETECTION_URL = 'http://localhost:6008/face-compare/cjjb'
# IMAGE = 'zidane.jpg'
IMAGE = '20200908221516829.png'
# IMAGE = "/home/yyf/Documents/PycharmProjects/yolov5/utils/flask_rest_api/vlcsnap-2023-11-14-14h18m20s882.png"
# IMAGE = "26ed6088dd203bbd7023a64fb263d251.png"
# Read image
with open(IMAGE, 'rb') as f:
    image_data = base64.b64encode(f.read()).decode("utf-8")

features = "-0.05306611955165863,0.12497729808092117,0.14246495068073273,-0.03874092549085617,-0.09271030128002167,0.00972053874284029,-0.005149582400918007,-0.11286775022745132,0.24372197687625885,-0.13178323209285736,0.19579419493675232,-0.0470384880900383,-0.2836962342262268,0.04499872028827667,-0.025788554921746254,0.28609487414360046,-0.1384579837322235,-0.15801800787448883,-0.16504305601119995,-0.09086499363183975,-0.017620401456952095,0.08484570682048798,-0.017698293551802635,0.06654389202594757,-0.11803184449672699,-0.3504123389720917,-0.008344934321939945,-0.06213178485631943,0.031429968774318695,-0.09642881155014038,0.01844888925552368,0.16512200236320496,-0.08050107210874557,0.034296244382858276,0.05238098278641701,0.10757515579462051,-0.09680783003568649,-0.14564725756645203,0.18265590071678162,0.03581617772579193,-0.3415268361568451,0.06250405311584473,0.11314994096755981,0.34029915928840637,0.28481170535087585,-0.010884743183851242,0.00476217083632946,-0.13318750262260437,0.1455094963312149,-0.4127466380596161,0.011031938716769218,0.10410240292549133,0.040483519434928894,0.11954262852668762,0.09796782582998276,-0.23354366421699524,0.04001634567975998,0.08355884999036789,-0.27875563502311707,0.03082229755818844,0.12169848382472992,-0.14494028687477112,-0.046806175261735916,-0.04911138489842415,0.25358614325523376,0.18742327392101288,-0.17851048707962036,-0.1399507075548172,0.16233235597610474,-0.21793997287750244,-0.05457090213894844,0.10412158817052841,-0.10435103625059128,-0.17751532793045044,-0.28012514114379883,0.020499592646956444,0.42976003885269165,0.15338410437107086,-0.1829841583967209,0.03518541157245636,-0.034885358065366745,0.03295157104730606,0.04370836541056633,0.1792171746492386,-0.03620016574859619,-0.12284346669912338,-0.07193456590175629,-0.03309687227010727,0.23611731827259064,-0.051643259823322296,0.01245851069688797,0.33571740984916687,0.045793503522872925,-0.08231820166110992,-0.05151958391070366,0.09387815743684769,-0.17931923270225525,-0.007034494541585445,-0.1942872554063797,-0.14111624658107758,0.033473625779151917,0.007580904755741358,0.043702058494091034,0.11501780897378922,-0.278068482875824,0.2928519546985626,-0.01499988604336977,0.016887225210666656,0.13164179027080536,-0.01104581169784069,-0.02404477447271347,-0.003309049177914858,0.19738779962062836,-0.2506413459777832,0.18256919085979462,0.175970196723938,0.12160424888134003,0.11765539646148682,0.08745196461677551,0.07283738255500793,0.029208995401859283,-0.04276477172970772,-0.18430432677268982,-0.0388154610991478,0.0584184005856514,-0.08462166786193848,-0.03376664221286774,0.05577419698238373;-0.05306611955165863,0.12497729808092117,0.14246495068073273,-0.03874092549085617,-0.09271030128002167,0.00972053874284029,-0.005149582400918007,-0.11286775022745132,0.24372197687625885,-0.13178323209285736,0.19579419493675232,-0.0470384880900383,-0.2836962342262268,0.04499872028827667,-0.025788554921746254,0.28609487414360046,-0.1384579837322235,-0.15801800787448883,-0.16504305601119995,-0.09086499363183975,-0.017620401456952095,0.08484570682048798,-0.017698293551802635,0.06654389202594757,-0.11803184449672699,-0.3504123389720917,-0.008344934321939945,-0.06213178485631943,0.031429968774318695,-0.09642881155014038,0.01844888925552368,0.16512200236320496,-0.08050107210874557,0.034296244382858276,0.05238098278641701,0.10757515579462051,-0.09680783003568649,-0.14564725756645203,0.18265590071678162,0.03581617772579193,-0.3415268361568451,0.06250405311584473,0.11314994096755981,0.34029915928840637,0.28481170535087585,-0.010884743183851242,0.00476217083632946,-0.13318750262260437,0.1455094963312149,-0.4127466380596161,0.011031938716769218,0.10410240292549133,0.040483519434928894,0.11954262852668762,0.09796782582998276,-0.23354366421699524,0.04001634567975998,0.08355884999036789,-0.27875563502311707,0.03082229755818844,0.12169848382472992,-0.14494028687477112,-0.046806175261735916,-0.04911138489842415,0.25358614325523376,0.18742327392101288,-0.17851048707962036,-0.1399507075548172,0.16233235597610474,-0.21793997287750244,-0.05457090213894844,0.10412158817052841,-0.10435103625059128,-0.17751532793045044,-0.28012514114379883,0.020499592646956444,0.42976003885269165,0.15338410437107086,-0.1829841583967209,0.03518541157245636,-0.034885358065366745,0.03295157104730606,0.04370836541056633,0.1792171746492386,-0.03620016574859619,-0.12284346669912338,-0.07193456590175629,-0.03309687227010727,0.23611731827259064,-0.051643259823322296,0.01245851069688797,0.33571740984916687,0.045793503522872925,-0.08231820166110992,-0.05151958391070366,0.09387815743684769,-0.17931923270225525,-0.007034494541585445,-0.1942872554063797,-0.14111624658107758,0.033473625779151917,0.007580904755741358,0.043702058494091034,0.11501780897378922,-0.278068482875824,0.2928519546985626,-0.01499988604336977,0.016887225210666656,0.13164179027080536,-0.01104581169784069,-0.02404477447271347,-0.003309049177914858,0.19738779962062836,-0.2506413459777832,0.18256919085979462,0.175970196723938,0.12160424888134003,0.11765539646148682,0.08745196461677551,0.07283738255500793,0.029208995401859283,-0.04276477172970772,-0.18430432677268982,-0.0388154610991478,0.0584184005856514,-0.08462166786193848,-0.03376664221286774,0.05577419698238373"

response = requests.post(DETECTION_URL, json={'image': image_data, 'features': features})
pprint.pprint(response.text)
