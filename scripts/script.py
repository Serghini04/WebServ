#!/usr/bin/env python3

import os
import cgi
import datetime

# Function to get a specific environment variable or return "Not available"
def get_env_var(var_name):
    return os.environ.get(var_name, "Not available")

def generate_response():
    # HTTP headers (these are passed via environment variables)
    request_method = get_env_var("REQUEST_METHOD")
    query_string = get_env_var("QUERY_STRING")
    content_length = get_env_var("CONTENT_LENGTH")
    content_type = get_env_var("CONTENT_TYPE")
    server_name = get_env_var("SERVER_NAME")
    server_port = get_env_var("SERVER_PORT")
    remote_addr = get_env_var("REMOTE_ADDR")
    user_agent = get_env_var("HTTP_USER_AGENT")
    referer = get_env_var("HTTP_REFERER")
    
    # CGI-specific details
    script_name = get_env_var("SCRIPT_NAME")
    script_filename = get_env_var("SCRIPT_FILENAME")
    gateway_interface = get_env_var("GATEWAY_INTERFACE")
    
    # Current timestamp
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # Start building the HTML response
    response = f"""
    <html>
    <head><title>CGI Example</title></head>
    <body>
    <h1>Welcome to the CGI Example</h1>
    <p>Current server time: {current_time}</p>

    <h2>Request Information</h2>
    <p><strong>Request Method:</strong> {request_method}</p>
    <p><strong>Query String:</strong> {query_string}</p>
    <p><strong>Content Length:</strong> {content_length}</p>
    <p><strong>Content Type:</strong> {content_type}</p>

    <h2>Server Information</h2>
    <p><strong>Server Name:</strong> {server_name}</p>
    <p><strong>Server Port:</strong> {server_port}</p>
    <p><strong>Script Name:</strong> {script_name}</p>
    <p><strong>Script Filename:</strong> {script_filename}</p>

    <h2>Client Information</h2>
    <p><strong>Client IP:</strong> {remote_addr}</p>
    <p><strong>User-Agent:</strong> {user_agent}</p>
    <p><strong>Referer:</strong> {referer}</p>

    <h2>Gateway Information</h2>
    <p><strong>Gateway Interface:</strong> {gateway_interface}</p>
    </body>
    </html>
    """
    return response

def main():
    # Send HTTP header (Content-Type + Status)
    print("Content-Type: text/html")
    print()  # Blank line separating header from body

    # Generate and print the response body
    print(generate_response())

if __name__ == "__main__":
    main()
