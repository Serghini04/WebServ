#!/usr/local/bin/python3

from http import cookies
import os
import cgi
import time
import hashlib
import pickle
import sys

class Session:
    def __init__(self, name):
        self.name = name
        self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
        with open('sessions/session_' + self.sid, 'wb') as f:
            pickle.dump(self, f)
    def getSid(self):
        return self.sid

""" Stores Users and thier data  """
class UserDataBase:
    def __init__(self):
        self.user_pass = {}
        self.user_firstname = {}
    def addUser(self, username, password, firstname):
        self.user_pass[username] = password
        self.user_firstname[username] = firstname
        with open('user_database', 'wb') as f:
            pickle.dump(self, f)


def printAccPage(session):
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html\r\n\r\n")
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Account Page</title>
        <style>
            :root {{
                --primary: #2563eb;
                --primary-hover: #1d4ed8;
                --success: #16a34a;
                --background: #f3f4f6;
                --text: #1f2937;
                --card: #ffffff;
            }}

            * {{
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                font-family: 'Segoe UI', system-ui, sans-serif;
            }}

            body {{
                background: var(--background);
                min-height: 100vh;
                display: flex;
                justify-content: center;
                align-items: center;
                text-align: center;
                padding: 2rem;
            }}

            .account-page {{
                background: var(--card);
                padding: 2rem;
                border-radius: 1rem;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                width: 100%;
                max-width: 600px;
            }}

            .account-page h1 {{
                font-size: 2rem;
                color: var(--text);
                margin-bottom: 1.5rem;
            }}

            .message-box {{
                padding: 1rem;
                border-radius: 0.5rem;
                font-size: 1rem;
                margin-bottom: 1.5rem;
                width: 100%;
                color: white;
            }}

            .success {{
                background: var(--success);
            }}

            .btn-primary {{
                display: inline-block;
                padding: 0.75rem 1.5rem;
                background: var(--primary);
                color: white;
                font-size: 1rem;
                font-weight: bold;
                border-radius: 0.5rem;
                text-decoration: none;
                transition: background 0.3s;
            }}

            .btn-primary:hover {{
                background: var(--primary-hover);
            }}
        </style>
    </head>
    <body>
        <div class="account-page">
            <h1>Welcome Back, {session.name}!</h1>
            <div class="message-box success">
                <p>Your Session ID: {session.getSid()}</p>
            </div>
            <h1> Upload Something : </h1>
            <form id="upload" enctype="multipart/form-data" method="POST" action="/upload">
                <input id="fileupload" name="myfile" type="file" multiple="multiple"/>
                <br>
                <input type="submit" value="Upload" id="submit"/>
            </form>
            <a href="login.html" class="btn-primary">Return to Homepage</a>
        </div>
    </body>
    </html>
    """)


def printUserMsg(msg):
    msg_class = "success" if "success" in msg.lower() else "error"
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html\r\n\r\n")
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>System Message</title>
        <style>
            :root {{
                --primary: #2563eb;
                --primary-hover: #1d4ed8;
                --error: #dc2626;
                --success: #16a34a;
                --background: #f3f4f6;
                --text: #1f2937;
                --card: #ffffff;
            }}

            * {{
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                font-family: 'Segoe UI', system-ui, sans-serif;
            }}

            body {{
                background: var(--background);
                min-height: 100vh;
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
                text-align: center;
                padding: 1rem;
            }}

            .message-box {{
                padding: 1rem;
                border-radius: 0.5rem;
                font-size: 1rem;
                text-align: center;
                margin: 1rem 0;
                width: 100%;
                max-width: 400px;
                color: white;
            }}

            .success {{
                background: var(--success);
            }}

            .error {{
                background: var(--error);
            }}

            .login-container {{
                background: var(--card);
                padding: 1rem;
                border-radius: 0.5rem;
                box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
                margin-top: 1rem;
            }}

            .btn-primary {{
                display: inline-block;
                padding: 0.75rem 1.5rem;
                background: var(--primary);
                color: white;
                font-size: 1rem;
                font-weight: bold;
                border-radius: 0.5rem;
                text-decoration: none;
                transition: background 0.3s;
            }}

            .btn-primary:hover {{
                background: var(--primary-hover);
            }}
        </style>
    </head>
    <body>
        <div class="message-box {msg_class}">
            <p>{msg}</p>
        </div>
        <div class="login-container">
            <a href="login.html" class="btn-primary">Return to Login</a>
        </div>
    </body>
    </html>
    """)


def printLogin():
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html\r\n\r\n")
    print("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Login Form</title>
            <style>
                :root {
                    --primary: #2563eb;
                    --primary-hover: #1d4ed8;
                    --background: #f3f4f6;
                    --text: #1f2937;
                    --card: #ffffff;
                }

                * {
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                    font-family: 'Segoe UI', system-ui, sans-serif;
                }

                body {
                    background: var(--background);
                    min-height: 100vh;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    padding: 1rem;
                }

                .login-container {
                    background: var(--card);
                    padding: 2rem;
                    border-radius: 1rem;
                    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                    width: 100%;
                    max-width: 400px;
                    text-align: center;
                }

                .login-title {
                    font-size: 1.75rem;
                    color: var(--text);
                    margin-bottom: 1rem;
                }

                .form-group {
                    text-align: left;
                    margin-bottom: 1rem;
                }

                .form-group label {
                    display: block;
                    font-size: 0.9rem;
                    margin-bottom: 0.5rem;
                    color: var(--text);
                }

                .form-group input {
                    width: 100%;
                    padding: 0.75rem;
                    border: 1px solid #d1d5db;
                    border-radius: 0.5rem;
                    font-size: 1rem;
                }

                .login-button {
                    width: 100%;
                    padding: 0.75rem;
                    background: var(--primary);
                    color: white;
                    font-size: 1rem;
                    font-weight: bold;
                    border: none;
                    border-radius: 0.5rem;
                    cursor: pointer;
                    transition: background 0.3s;
                }

                .login-button:hover {
                    background: var(--primary-hover);
                }

                .register-link {
                    margin-top: 1rem;
                    font-size: 0.9rem;
                }

                .register-link a {
                    color: var(--primary);
                    text-decoration: none;
                    font-weight: 500;
                }

                .register-link a:hover {
                    text-decoration: underline;
                }
            </style>
        </head>
        <body>
            <main class="login-container">
                <h1 class="login-title">Welcome Back</h1>
                <form action="acc.py" method="get">
                    <div class="form-group">
                        <label for="username">Username:</label>
                        <input type="text" id="username" placeholder="Enter Username" name="username" required>
                    </div>
                    <div class="form-group">
                        <label for="password">Password:</label>
                        <input type="password" id="password" placeholder="Enter Password" name="password" required>
                    </div>
                    <button type="submit" class="login-button">Sign In</button>
                    <p class="register-link">No account? <a href="register.html">Create one</a></p>
                </form>
            </main>
        </body>
        </html>
    """)


def authUser(name, password):
    if os.path.exists('user_database'):
        with open('user_database', 'rb') as f:
            database = pickle.load(f)
            if name in database.user_pass and database.user_pass[name] == password:
                session = Session(database.user_firstname[name])
                return session
            else:
                return None
    else:
        return None

def handleLogin():
    username = form.getvalue('username')
    password = form.getvalue('password')
    firstname = form.getvalue('firstname')
    
    if username is None:
        printLogin()
    elif firstname is None:
        session = authUser(username, password)
        if session is None:
            printUserMsg("Failed To Login, Username or Password is wrong!")
        else:
            # Initialize or reset the cookies
            cookies_obj = cookies.SimpleCookie()

            # Manually expire the "SID" cookie if it exists
            if "SID" in cookies_obj:
                cookies_obj["SID"]["expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"  # Expire the cookie

            # Set a new "SID" cookie with session ID
            cookies_obj["SID"] = session.getSid()
            cookies_obj["SID"]["expires"] = 120  # Set the cookie to expire in 2 minutes
            
            # Output the cookie headers
            print("HTTP/1.1 302 Found")
            print(cookies_obj.output())
            print("Content-Type: text/html")
            print("Location: acc.py\r\n\r\n")
    else:
        # Registration process - without username check
        try:
            database = None
            if os.path.exists('user_database'):
                with open('user_database', 'rb') as f:
                    database = pickle.load(f)
            else:
                database = UserDataBase()
                
            # Add new user without checking if username exists
            database.addUser(username, password, firstname)
            printUserMsg("Account registered successfully!")
        except Exception as e:
            # Debug output
            printUserMsg(f"Error during registration: {str(e)}") 

def main():
    # Create sessions directory if it doesn't exist
    if not os.path.exists('sessions'):
        os.makedirs('sessions')
        
    global form
    # Load CGI form
    form = cgi.FieldStorage()

    # Check if we're processing a login attempt
    login_attempt = form.getvalue('username') is not None

    # If we're processing a login attempt, prioritize that over the cookie
    if login_attempt:
        handleLogin()
    else:
        # Only check for the cookie if we're not processing a login
        cookies_obj = cookies.SimpleCookie()
        if 'HTTP_COOKIE' in os.environ:
            cookies_obj.load(os.environ['HTTP_COOKIE'])

            if "SID" in cookies_obj:
                sid = cookies_obj["SID"].value
                session_file = 'sessions/session_' + sid
                
                # Check if the session file exists
                if os.path.exists(session_file):
                    with open(session_file, 'rb') as f:
                        sess = pickle.load(f)
                    printAccPage(sess)
                else:
                    # Session file doesn't exist, handle as if no cookie
                    handleLogin()
            else:
                handleLogin()
        else:
            handleLogin()

if __name__ == "__main__":
    main()
