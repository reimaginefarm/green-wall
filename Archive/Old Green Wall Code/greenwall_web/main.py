from flask import Flask
app = Flask(__name__)


@app.route('/')
def index():

    return '<html><body><h1>Green Wall Control Panel</h1></body></html>';

@app.route("/on/")
def on():

    return '<a href="/off">Off</a>';

@app.route("/off/")
def off():

    return '<a href="/on">On</a>';



if __name__ == '__main__':
    app.debug = True
    app.run(host= '192.168.10.126')
