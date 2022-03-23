from flask import Flask
from nest import Nest

import os

nest = Nest(client_id=os.environ.get("CLIENT_ID"), 
            client_secret=os.environ.get("CLIENT_SECRET"), 
            code=os.environ.get("CODE"))
app = Flask(__name__)

@app.route("/stats")
def stats():
    return nest.get_stats()

@app.route("/eco_off", methods = ["POST"])
def turn_off():
    return nest.set_eco_off()

@app.route("/eco_on", methods = ["POST"])
def turn_on():
    return nest.set_eco_on()

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
