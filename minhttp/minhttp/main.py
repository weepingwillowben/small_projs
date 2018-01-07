#!/usr/bin/env python

from flask import Flask, render_template, request

from minhttp import db, app
from minhttp import schema

@app.route('/add_info')
def add_info():
    return render_template("add_info.html")


@app.route('/')
@app.route('/index.html')
@app.route('/list_info')
def get_info():
    all_infos = schema.Info.query.all()
    return render_template('info.html',infos=all_infos)

@app.route('/newinfo', methods=['POST'])
def new_info():
    db_entry = schema.Info(
        info=request.form['info']
    )
    db.session.add(db_entry)
    db.session.commit()

    return 'Okay, added an info! "{}" has been saved'.format(
        db_entry.info
    )
