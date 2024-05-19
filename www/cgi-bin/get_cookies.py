#!/usr/bin/python3

import os
from http import cookies
# Import modules for CGI handling 
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
cookie_name = form.getvalue('cookieName')

print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print("\r\n")

print("""
<html>
<head>
<title>Get Cookies</title>
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

.cookie {
    background-color: #fff;
    border: 1px solid #ddd;
    padding: 10px;
    margin-bottom: 10px;
    border-radius: 5px;
}

button, input[type="submit"] {
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
    border-radius: 4px;
    transition: all 0.3s;
}

button:hover, input[type="submit"]:hover {
    background-color: #45a049;
}

input[type="text"] {
    width: 100%;
    padding: 12px 20px;
    margin: 8px 0;
    box-sizing: border-box;
    border: 2px solid #ccc;
    border-radius: 4px;
    transition: 0.3s;
}

input[type="text"]:focus {
    border-color: #4CAF50;
}

input[type="submit"].delete-button {
    background-color: #f44336; /* Red */
    border: none;
    color: white;
    padding: 6px 12px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 12px;
    margin: 4px 2px;
    cursor: pointer;
    border-radius: 4px;
    transition: all 0.3s;
}

input[type="submit"].delete-button:hover {
    background-color: #d32f2f;
}

</style>
</head>
<body>
<div class="container">
<h1>Get Cookies</h1>
<form action="/cgi-bin/get_cookies.py" method="get">
  <label for="cookieName">Cookie Name:</label><br>
  <input type="text" id="cookieName" name="cookieName"><br>
  <input type="submit" value="Submit">
</form> 
""")

# Get the cookies sent in the HTTP request
if 'HTTP_COOKIE' in os.environ:
    cookie_string = os.environ['HTTP_COOKIE']
    cookie = cookies.SimpleCookie()
    cookie.load(cookie_string)

    # Retrieve a specific cookie
    if cookie_name in cookie:
        print(f'<div class="cookie"><strong>{cookie_name}</strong> = {cookie[cookie_name].value}</div>')
    else:
        print(f'<p>No cookie named <strong>{cookie_name}</strong></p>')

    # Print all cookies
    print("<h2>All Cookies:</h2>")
    for key, morsel in cookie.items():
        if key != 'csrftoken':
            print(f'<div class="cookie"><strong>{key}</strong></div>')
            print(f'<form action="/cgi-bin/delete_cookies.py" method="post">')
            print(f'<input type="hidden" name="cookieName" value="{key}">')
            print('<input type="submit" value="Delete" class="delete-button">')  # Assign the 'delete-button' class
            print('</form>')
else:
    print("<p>No cookies sent in the request</p>")

print("""
<button onclick="window.history.back();">Go Back</button>
</div>
</body>
</html>
""")