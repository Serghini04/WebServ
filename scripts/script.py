#!/usr/bin/python3

import os

# Get the query string from the environment
query_string = os.getenv("QUERY_STRING", "No query provided")

# Print HTTP headers
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print("\n\r\n\r")  # Blank line to separate headers from body

# Print HTML response
pr**int("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from CGI!</h1>")
print(f"<p>Hello Mr {query_string}</p>")
print("</body>")
print("</html>")
