from minhttp import app

app.config["DEBUG"] = True
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///canyon_db.sqlite"
