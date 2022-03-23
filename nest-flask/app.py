from flask import Flask
from nest import Nest

import os

nest = Nest(client_id=os.environ.get("CLIENT_ID"), 
            client_secret=os.environ.get("CLIENT_SECRET"), 
            code=os.environ.get("CODE"))
app = Flask(__name__)

@app.route("/stats")
def hello_world():
    return nest.get_stats()


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
