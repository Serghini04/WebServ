#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb

# Enable error debugging
cgitb.enable()

def get_form_data():
    """ Parse multipart form data from stdin """
    form = cgi.FieldStorage(fp=sys.stdin, environ=os.environ, keep_blank_values=True)

    # Extract form fields
    name = form.getvalue("name", "Not provided")
    password = form.getvalue("password", "Not provided")

    return name, password

def generate_response(name, password):
    """ Generate HTML response """
    return f"""\
    <html>
    <head><title>CGI Form Output</title></head>
    <body>
    <h1>Form Data</h1>
    <p><strong>Name:</strong> {name}</p>
    <p><strong>Password:</strong> {password}</p>
    </body>
    </html>
    """

def main():
    # Print HTTP response header
    print("Content-Type: text/html")
    print()  # Empty line separating headers from content

    # Get form data from request body
    name, password = get_form_data()

    # Print the response
    print(generate_response(name, password))

if __name__ == "__main__":
    main()
