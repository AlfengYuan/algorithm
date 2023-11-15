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

features = [{'searchValue': None,
             'createBy': None,
             'createTime': None,
             'updateBy': None,
             'updateTime': None,
             'remark': None,
             'params': {},
             'id': 10,
             'name': '测试444',
             'certificate': None,
             'phone': None,
             'image': '3176',
             'addTime': None,
             'feature': '-0.09300774335861206,0.12678611278533936,0.027764325961470604,-0.0819174125790596,-0.11008551716804504,-0.05863562226295471,-0.11702713370323181,-0.1133645623922348,0.16651132702827454,-0.15149512887001038,0.2022629827260971,-0.07525927573442459,-0.19699858129024506,-0.006694145500659943,-0.13845543563365936,0.2650400996208191,-0.18252350389957428,-0.15299774706363678,-0.0856902226805687,0.020461518317461014,0.0724172443151474,-0.002444016747176647,-0.04630276933312416,0.09823444485664368,-0.08561111241579056,-0.33485889434814453,-0.027934269979596138,-0.0156763456761837,-0.07022567093372345,-0.10446931421756744,0.0077674295753240585,0.03935040533542633,-0.15795697271823883,0.008547134697437286,0.009172922000288963,0.06780117005109787,-0.03072943724691868,-0.09337089210748672,0.13736578822135925,0.027826476842164993,-0.3078310787677765,-0.006878959480673075,0.09760365635156631,0.2650624215602875,0.19740056991577148,0.011826010420918465,0.03066077083349228,-0.17866726219654083,0.1465906947851181,-0.21993502974510193,-0.015074152499437332,0.1162002682685852,0.03853140026330948,0.0828763023018837,0.051885202527046204,-0.17664824426174164,0.007459904998540878,0.1514567881822586,-0.115950807929039,0.018840039148926735,0.055611658841371536,-0.03352179378271103,-0.035049498081207275,-0.17118676006793976,0.2373589724302292,0.11908339709043503,-0.14859923720359802,-0.2156180888414383,0.1483316421508789,-0.16112947463989258,-0.12930285930633545,0.059017814695835114,-0.12157060205936432,-0.20679467916488647,-0.3589053452014923,-0.059740740805864334,0.2675951421260834,0.16412608325481415,-0.14845727384090424,0.037837207317352295,0.008752936497330666,0.014144076965749264,0.03736836463212967,0.2016971856355667,0.004623580724000931,0.05831556022167206,-0.031962331384420395,-0.007603832520544529,0.22715941071510315,-0.028991907835006714,-0.02843947894871235,0.23404516279697418,-0.050527069717645645,0.0030348096042871475,-0.07778728008270264,0.02419920079410076,-0.14675021171569824,0.06834350526332855,-0.14803799986839294,-0.06158122420310974,0.030821451917290688,0.05228160321712494,0.030670199543237686,0.1370512992143631,-0.1500566303730011,0.13576656579971313,-0.04115556180477142,0.09403830021619797,0.018752576783299446,0.06128997728228569,-0.08229000121355057,-0.04785501956939697,0.13593539595603943,-0.1530327945947647,0.22791942954063416,0.1723734587430954,0.1511259377002716,0.04940829426050186,0.1559673398733139,0.040646180510520935,0.01567741483449936,-0.006318916566669941,-0.21595463156700134,0.013766005635261536,0.055449943989515305,-0.08657148480415344,0.062218766659498215,0.01767485961318016'}]
response = requests.post(DETECTION_URL, json={'image': image_data, 'features': features})
pprint.pprint(response.text)
