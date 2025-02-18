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
    print("Content-Type: text/html\r\n\r\n")
    print("<html>")
    print("<head>")
    print("<title>Account Page</title>")
    print("</head>")
    print("<body>")
    print("<h1>Welcome Again", session.name, "!</h1>")
    print("<p>Your Session ID is: ", session.getSid(), "</p>")
    print("</body>")
    print("<a href=\"index.html\"> Click here to go back to homepage </a>")
    print("</html>")

def printUserMsg(msg):
    print("Content-Type: text/html\r\n\r\n")
    print("<html>")
    print("<head>")
    print("<title>USER MSG</title>")
    print("</head>")
    print("<body>")
    print("<h1>", msg ,"</h1>")
    print("</body>")
    print("<a href=\"login.html\"> Click here to go back to login page </a>")
    print("</html>")

def printLogin():
    print("Content-Type: text/html\r\n\r\n")
    print("<html> ")
    print("<head>")
    print("<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">")
    print("<link rel=\"stylesheet\" href=\"assets/css/accstyle.css\">")
    print("<title> Login Page </title>")
    print("</head>")
    print("<body>  ")
    print("<center> <h1> Login Form </h1> </center> ")
    print("<form action = \"acc.py\" method = \"get\">")
    print("<div class=\"container\"> ")
    print("<label>Username : </label> ")
    print("<input Type=\"text\" placeholder=\"Enter Username\" name=\"username\" required>")
    print("<label>Password : </label> ")
    print("<input Type=\"password\" placeholder=\"Enter Password\" name=\"password\" required>")
    print("<button Type=\"submit\">Login</button> ")
    print("No Account?<a href=\"register.html\"> Register Here </a> ")
    print("</div> ")
    print("</form>   ")
    print("</body>   ")
    print("</html>")




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
            print("Correct Credentials :D", file=sys.stderr)

            # Initialize or reset the cookies
            cookies_obj = cookies.SimpleCookie()

            # Manually expire the "SID" cookie if it exists
            if "SID" in cookies_obj:
                cookies_obj["SID"]["expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"  # Expire the cookie

            # Set a new "SID" cookie with session ID
            cookies_obj["SID"] = session.getSid()
            cookies_obj["SID"]["expires"] = 120  # Set the cookie to expire in 2 minutes
            
            # Output the cookie headers
            print(cookies_obj.output())
            print("Location: acc.py")
            print("\r\n\r\n")
    else:
        if os.path.exists('user_database'):
            with open('user_database', 'rb') as f:
                database = pickle.load(f)
                if username in database.user_pass:
                    printUserMsg("Username is already registered!")
                else:
                    database.addUser(username, password, firstname)
                    printUserMsg("Account registered successfully!")
        else:
            database = UserDataBase()
            if username in database.user_pass:
                printUserMsg("Username is already registered!")
            else:
                database.addUser(username, password, firstname)
                printUserMsg("Account registered successfully!")

# Load CGI form
form = cgi.FieldStorage()

# Load cookies from the environment
cookies_obj = cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookies_obj.load(os.environ['HTTP_COOKIE'])

    if "SID" in cookies_obj:
        print("Your Session ID is", cookies_obj["SID"].value, file=sys.stderr)
        with open('sessions/session_' + cookies_obj["SID"].value, 'rb') as f:
            sess = pickle.load(f)
        printAccPage(sess)
    else:
        handleLogin()
else:
    handleLogin()
