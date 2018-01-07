from server import app
from flask import request, render_template, redirect, url_for
from server import game

@app.route("/")
def login():
    return render_template("main.html")

#@app.route("/load_game", methods=["POST"])
@app.route("/start_new_game", methods=["POST"])
def start_game():
    game.new_game(500,500)
    return game.cur_game.to_json()
