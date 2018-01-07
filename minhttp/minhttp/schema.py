from datetime import datetime
from minhttp import db

class IdPrimaryKeyMixin(object):
    id = db.Column(db.Integer, primary_key=True)


class DateTimeMixin(object):
    created_on = db.Column(db.DateTime, default=datetime.now)
    updated_on = db.Column(db.DateTime, default=datetime.now, onupdate=datetime.now)


class Info(db.Model, IdPrimaryKeyMixin, DateTimeMixin):
    info = db.Column(db.Text, nullable=False)
