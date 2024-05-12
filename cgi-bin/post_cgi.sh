#!/bin/bash

# Set the URL of your local CGI script
url="http://localhost/cgi-bin/your_cgi_script.cgi"

# Set the data you want to send in the POST request
data="data=value1"

# Send the POST request using curl
response=$(curl -s -X POST -d "$data" "$url")

# Print the response from the server
echo "Response from server:"
echo "$response"
