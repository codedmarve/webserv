#!/usr/bin/python3

import os
import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get the directory name to delete from the form
directory_to_delete = form.getvalue('filename')

# Set the base directory where submissions are stored
base_directory = os.path.join(os.getcwd(), 'www', 'serverDB')
# Check if directory_to_delete is None
if directory_to_delete is None:
    message = 'No directory name provided.'
else:
    # Delete the directory if it exists
    directory_path = os.path.join(base_directory, directory_to_delete)
    if os.path.exists(directory_path):
        # Delete the entire directory and its contents
        import shutil
        shutil.rmtree(directory_path)
        message = f'The directory "{directory_to_delete}" has been successfully deleted.'
    else:
        message = f'Directory "{directory_to_delete}" does not exist or could not be deleted.'

# Output the result
print("Content-Type: text/html;charset=utf-8\r\n")
print()
print("<html><body>")
print(f"<h1>{message}</h1>")
print("</body></html>")
