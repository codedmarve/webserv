#!/usr/bin/python3

import cgi
import os
import uuid
import json

def handle_file_upload(form, current_directory):
    if 'uploadFile' in form and form['uploadFile'].filename:
        fileitem = form['uploadFile']
        
        filename_base = os.path.basename(fileitem.filename)
        
        unique_folder_name = str(uuid.uuid4())
        submission_directory = os.path.join(current_directory, 'serverDB', unique_folder_name)
        os.makedirs(submission_directory, exist_ok=True)

        save_path = os.path.join(submission_directory, filename_base)

        with open(save_path, 'wb') as f:
            f.write(fileitem.file.read())
        
        return submission_directory, f'The file "{filename_base}" was uploaded to {save_path}'
    else:
        return None, 'No file uploaded or field name "uploadFile" is missing'

def save_form_fields_as_json(form, submission_directory):
    json_file_path = os.path.join(submission_directory, 'submission_details.json')
    form_data = {key: form.getvalue(key) for key in form.keys() if key != 'uploadFile'}
    
    with open(json_file_path, 'w') as json_file:
        json.dump(form_data, json_file, indent=4)
    
    return json_file_path

form = cgi.FieldStorage()

current_directory = os.getcwd() + '/www'

submission_directory, _ = handle_file_upload(form, current_directory)

if submission_directory:
    save_form_fields_as_json(form, submission_directory)

response = {
    "submission_directory": submission_directory if submission_directory else None
}

print("Content-Type: application/json;charset=utf-8")
print()
print(json.dumps(response, indent=4))
