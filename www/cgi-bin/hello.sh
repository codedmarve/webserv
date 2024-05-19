#!/bin/bash

# Set the Content-Type header to indicate that the response is HTML
echo "Content-Type: text/html"
echo -e "\r\n\r\n"
echo ""

# Output the HTML content
echo "<html>"
echo "<head>"
echo "<title>CGI Bash Example</title>"
echo "</head>"
echo "<body>"
echo "<h1>Hello, World!</h1>"
echo "<p>This is a CGI script written in Bash.</p>"
echo "</body>"
echo "</html>"
