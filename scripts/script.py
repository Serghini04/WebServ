#!/usr/bin/env python3
import os
import sys
import cgitb
cgitb.enable()

def parse_multipart_form(post_data, content_type):
    data = {}
    
    # Extract boundary from Content-Type header
    boundary = content_type.split("boundary=")[-1].strip()
    boundary = '--' + boundary  # Boundaries in data start with --
    
    # Split parts using the full boundary
    parts = post_data.split(boundary)
    
    for part in parts:
        part = part.strip()
        if not part or part == '--':  # Skip empty parts and the ending --
            continue
        
        # Split headers and body
        headers_body = part.split('\r\n\r\n', 1)
        if len(headers_body) < 2:
            continue
        
        headers, body = headers_body
        headers = headers.split('\r\n')
        
        # Extract field name from headers
        field_name = None
        for header in headers:
            if 'Content-Disposition:' in header:
                name_start = header.find('name="') + 6
                name_end = header.find('"', name_start)
                field_name = header[name_start:name_end]
                break
        
        if field_name:
            data[field_name] = body.strip()  # Get the value after headers
    
    return data

def main():
    print("Content-Type: text/html\n")
    
    try:
        if os.environ.get('REQUEST_METHOD') == 'POST':
            content_length = int(os.environ.get('CONTENT_LENGTH', 0))
            content_type = os.environ.get('CONTENT_TYPE', '')
            
            # Read raw POST data
            post_data = sys.stdin.buffer.read(content_length).decode('utf-8')
            
            # Parse the multipart form data
            form_data = parse_multipart_form(post_data, content_type)
            
            # Generate HTML response
            print("<html>")
            print("<head><title>Form Data Results</title></head>")
            print("<body>")
            print("<h1>Received Form Data:</h1>")
            
            if form_data:
                username = form_data.get('username', 'Not provided')
                password = form_data.get('password', 'Not provided')
                print(f"<p><strong>Username:</strong> {username}</p>")
                print(f"<p><strong>Password:</strong> {password}</p>")
            else:
                print("<p>No form data received</p>")
                
            print("</body></html>")
        else:
            print("<html><body>Only POST method is supported</body></html>")
            
    except Exception as e:
        print("<html><body>")
        print(f"<h1>Error</h1><p>{str(e)}</p>")
        print("</body></html>")

if __name__ == "__main__":
    main()