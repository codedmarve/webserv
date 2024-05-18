#!/usr/bin/python3

import cgi
import os

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Print HTTP headers
print("Content-Type: text/html;charset=utf-8")
print()

# Get filename here
if 'uploadFile' in form and form['uploadFile'].filename:
    fileitem = form['uploadFile']

    # Secure the file path
    current_directory = os.getcwd() + '/www/'
    print("<p>Current Directory: " + current_directory + "</p>")
    
    filename_base = os.path.basename(fileitem.filename)
    print("<p>Filename Base: " + filename_base + "</p>")
    
    save_path = os.path.join(current_directory, 'serverDB', filename_base)
    print("<p>Save Path: " + save_path + "</p>")
    
    # Open and write the file
    with open(save_path, 'wb') as f:
        f.write(fileitem.file.read())
    
    message = 'The file "' + filename_base + '" was uploaded to ' + save_path
else:
    message = 'No file uploaded or field name "uploadFile" is missing'

# Output the message
print("<html><body>")
print(f"<h1>{message}</h1>")
print("</body></html>")
