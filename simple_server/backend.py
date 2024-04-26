#!/usr/bin/env python

import cgi
import logging
import urllib

import webapp2

from google.appengine.api import memcache
from google.appengine.ext import ndb

from google.appengine.api import mail

PARAM_DEFAULT_VALUE='aniovawehn348cnawfNYOCER #R*R*&^*#:{{|{{<>:<'

class KeyValue(ndb.Model):
  kv_id = ndb.StringProperty()
  kv_key = ndb.StringProperty()
  kv_value = ndb.StringProperty();

  @classmethod
  def query_for_id_and_key(cls, kv_id, kv_key):
    return cls.query(cls.kv_id == kv_id, cls.kv_key == kv_key)

class MainPage(webapp2.RequestHandler):
  def get(self):
    self.response.out.write("""
    /setkeyvalue?id=id&k=key&v=value<br>
    /cachekeyvalue?id=id&k=key&v=value<br>
    /getkeyvalue?id=id&k=key - searches cached, then set values<br>
    /viewkeyvalues""")

class SetKeyValue(webapp2.RequestHandler):
  def get(self):
    kv_id = self.request.get('id', default_value=PARAM_DEFAULT_VALUE)
    if kv_id == PARAM_DEFAULT_VALUE:
        self.response.out.write("ERROR: Need id=");
        return
    kv_key = self.request.get('k', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
        kv_key = self.request.get('key', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
        self.response.write('ERROR: Need k= or key=')
        return
    kv_value = self.request.get('v', default_value=PARAM_DEFAULT_VALUE)
    if kv_value == PARAM_DEFAULT_VALUE:
        kv_value = self.request.get('value')
    if kv_value == PARAM_DEFAULT_VALUE:
        self.response.write('ERROR: Need v= or value=')
        return
    memcache.set('id=' + kv_id + ' key=' + kv_key, kv_value)
    kv = KeyValue.query_for_id_and_key(kv_id = kv_id, kv_key = kv_key).fetch(1)
    if len(kv) > 0:
        kv[0].kv_value = kv_value
        kv[0].put()
    else:
        kv = KeyValue(kv_id = kv_id, kv_key = kv_key, kv_value = kv_value)
        kv.put()
    self.response.write('OK')

class CacheKeyValue(webapp2.RequestHandler):
  def get(self):
    kv_id = self.request.get('id', default_value=PARAM_DEFAULT_VALUE)
    if kv_id == PARAM_DEFAULT_VALUE:
        self.response.out.write("ERROR: Need id=");
        return
    kv_key = self.request.get('k', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
        kv_key = self.request.get('key', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
        self.response.write('ERROR: Need k= or key=')
        return
    kv_value = self.request.get('v', default_value=PARAM_DEFAULT_VALUE)
    if kv_value == PARAM_DEFAULT_VALUE:
        kv_value = self.request.get('value')
    if kv_value == PARAM_DEFAULT_VALUE:
        self.response.write('ERROR: Need v= or value=')
        return
    memcache.set('id=' + kv_id + ' key=' + kv_key, kv_value)
    self.response.write('OK')

class GetKeyValue(webapp2.RequestHandler):
  def get(self):
    kv_id = self.request.get('id', default_value=PARAM_DEFAULT_VALUE)
    if kv_id == PARAM_DEFAULT_VALUE:
      self.response.out.write("ERROR: Need id=");
      return
    kv_key = self.request.get('k', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
      kv_key = self.request.get('key', default_value=PARAM_DEFAULT_VALUE)
    if kv_key == PARAM_DEFAULT_VALUE:
      self.response.write('ERROR: Need k= or key=')
      return
    kv_value = memcache.get('id=' + kv_id + ' key=' + kv_key)
    if kv_value is not None:
        self.response.write(kv_value)
        return
    kv = KeyValue.query_for_id_and_key(kv_id = kv_id, kv_key = kv_key).fetch(1)
    if len(kv) > 0:
      self.response.write(kv[0].kv_value)

class ViewKeyValues(webapp2.RequestHandler):
  def get(self):
    kvs = KeyValue.query().iter();
    while kvs.has_next():
      kv = kvs.next()
      self.response.write("id=" + kv.kv_id + " key=" + kv.kv_key + " value=" + kv.kv_value)

app = webapp2.WSGIApplication([
  ('/', MainPage),
  ('/getkeyvalue', GetKeyValue),
  ('/setkeyvalue', SetKeyValue),
  ('/cachekeyvalue', CacheKeyValue),
  ('/viewkeyvalues', ViewKeyValues)
], debug=True)
