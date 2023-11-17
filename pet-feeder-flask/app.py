from flask import Flask
from feeder import Feeder

import os

feeder = Feeder(ip=os.environ.get("FEEDER_IP"), 
            dev_id=os.environ.get("FEEDER_DEV_ID"), 
            local_key=os.environ.get("FEEDER_LOCAL_KEY"))
app = Flask(__name__)

@app.route("/status")
def stats():
    return feeder.get_status()

@app.route("/feed", methods = ["POST"])
def feed():
    feeder.give_food()
    return ""

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8080)
