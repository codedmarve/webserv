#!/usr/bin/python3

import cgi
import os
import uuid

def handle_file_upload(form, current_directory):
    if 'uploadFile' in form and form['uploadFile'].filename:
        fileitem = form['uploadFile']
        
        filename_base = os.path.basename(fileitem.filename)
        
        unique_folder_name = str(uuid.uuid4())  # Generate a UUID
        submission_directory = os.path.join(current_directory, 'serverDB', unique_folder_name)
        os.makedirs(submission_directory, exist_ok=True)

        save_path = os.path.join(submission_directory, filename_base)
        
        # Open and write the file
        with open(save_path, 'wb') as f:
            f.write(fileitem.file.read())
        
        message = f'The file "{filename_base}" was uploaded to {save_path}'
        return submission_directory, message
    else:
        return None, 'No file uploaded or field name "uploadFile" is missing'

def save_form_fields(form, submission_directory):
    text_file_path = os.path.join(submission_directory, 'submission_details.txt')
    with open(text_file_path, 'w') as text_file:
        for key in form.keys():
            if key != 'uploadFile':  # Exclude the uploaded file from the text file
                # Write each form field and its value to the text file
                text_file.write(f"{key}: {form.getvalue(key)}\n")
    return text_file_path

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get current directory
current_directory = os.getcwd() + '/www'

# Handle file upload
submission_directory, message = handle_file_upload(form, current_directory)

# Save form fields
if submission_directory:
    text_file_path = save_form_fields(form, submission_directory)
    message += f'<br>Submission details saved in: {text_file_path}'

# Output the message with appropriate HTTP headers
print("Content-Type: text/html;charset=utf-8")
print()
print("<html><body>")
print(f"<h1>{message}</h1>")
print("</body></html>")
