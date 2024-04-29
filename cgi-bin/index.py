#!/usr/bin/python3

import os
import cgi

print("Status: 200 OK\r\n\r\n")
print("Content-type: text/html\r\n\r\n")

print("<html>")
print("<head>")
print("<h2>Environment:</h2><br>")


print("<html>")
print("<body>")
for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

print("</body>")
print("</html>")