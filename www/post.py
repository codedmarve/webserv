#!/usr/bin/python3
import cgi
import cgitb
import json

cgitb.enable()
print("HTTP/1.1 200 OK")
print("Content-Type: application/json;charset=utf-8\r\n\r\n")
print()

form = cgi.FieldStorage()

response = {
    "message": "POST request received",
    "data": {}
}

for key in form.keys():
    response["data"][key] = form.getvalue(key)

print(json.dumps(response))