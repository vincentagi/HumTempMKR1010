import time
from urllib.request import urlopen
import json
from datetime import datetime

# store the URL in url as 
# parameter for urlopen
url = "http://YOUR_IP/"
filename = "dataanalog.json"
waitInSecond = 5

while 1:
    response = urlopen(url)
    data_json = json.loads(response.read())
    now = datetime.now()
    time_record = {"datetime": now.strftime("%d/%m/%Y %H:%M:%S")}
    with open(filename, 'a+') as data_file:
        data_json.update(time_record)
        data_file.write(str(data_json))
        data_file.write(',')
        data_file.write('\n')
    time.sleep(waitInSecond)