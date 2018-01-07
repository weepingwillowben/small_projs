from flask import Flask
import os

app = Flask(__name__)

#app.config['SQLALCHEMY_DATABASE_URI']  = "postgres://localhost/strabo"
app.config['SQLALCHEMY_TRACK_MODIFICATIONS']  = False
app.config['DEBUG']  = True

import server.views
