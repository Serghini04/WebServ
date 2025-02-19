#!/usr/bin/env python3
import os
import sys

# Ensure script prints output immediately
sys.stdout.flush()

# Print required CGI headers
print("HTTP/1.1 200 OK\r\n") # Separate headers from body
print("Content-Length: 115\r\n") # Separate headers from body
print("Content-Type: text/html\r\n\r\n") # Separate headers from body

# HTML response body
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from CGI!</h1>")
print("<p>Query String: {}</p>".format(os.getenv("QUERY_STRING", "No query provided")))
print("</body>")
print("</html>")
