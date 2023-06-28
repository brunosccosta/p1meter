from flask import Flask
from epex import EPEXSpotWeb

import os

epex = EPEXSpotWeb('NL')
app = Flask(__name__)

@app.route("/")
def stats():
    return epex.fetch_json()

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5001)
