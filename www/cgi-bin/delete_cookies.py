#!/usr/bin/python3

import os
import cgi
from http import cookies

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
cookie_name = form.getvalue('cookieName')

print("HTTP/1.1 200 OK")
print(f'Set-Cookie: {cookie_name}=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT')
print("Content-type: text/html")
print("\r\n")

print("""
<html>
<head>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f0f0f0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
        h1 {
            margin-bottom: 20px;
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
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <div>
        <h1>Cookie has been deleted.</h1>
        <button onclick="window.history.back();">Go Back</button>
    </div>
</body>
</html>
""")