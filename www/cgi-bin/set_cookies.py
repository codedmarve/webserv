#!/usr/bin/python3

import os
from http import cookies
# Import modules for CGI handling 
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
name = form.getvalue('name')
value  = form.getvalue('value')
cookie = cookies.SimpleCookie()
cookie[name] = value
print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print(cookie.output())
print("\r\n")

print("""
<html>
<head>
<title>Set Cookie</title>
<style>
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    background-color: #f0f0f0;
}

.container {
    width: 80%;
    margin: 0 auto;
    padding: 20px;
}

h1 {
    color: #333;
}

p {
    color: #666;
}

button {
    background-color: #4CAF50; /* Green */
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 2px;
    cursor: pointer;
}
</style>
</head>
<body>
<div class="container">
<h1>Set Cookie</h1>
<p>Cookie has been set.</p>
<button onclick="window.history.back();">Go Back</button>
</div>
</body>
</html>
""")