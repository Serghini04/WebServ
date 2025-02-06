#!/usr/local/bin/python3
import os
import cgi
import hashlib
import pickle

class UserDataBase:
    def __init__(self):
        self.user_pass = {}
        self.user_firstname = {}
        
    def hash_password(self, password):
        return hashlib.sha256(password.encode('utf-8')).hexdigest()
        
    def addUser(self, username, password, firstname):
        if not username or not password or not firstname:
            return False, "All fields are required"
            
        hashed_password = self.hash_password(password)
        self.user_pass[username] = hashed_password
        self.user_firstname[username] = firstname
        
        with open('user_database', 'wb') as f:
            pickle.dump(self, f)
        return True, "User registered successfully"
        
    def validate_user(self, username, password):
        if not os.path.exists('user_database'):
            return False
            
        with open('user_database', 'rb') as f:
            database = pickle.load(f)
            
        hashed_password = self.hash_password(password)
        return (username in database.user_pass and
                database.user_pass[username] == hashed_password)
                
    def get_firstname(self, username):
        if not os.path.exists('user_database'):
            return None
            
        with open('user_database', 'rb') as f:
            database = pickle.load(f)
            
        return database.user_firstname.get(username)

def print_result(success, message, is_login=True, username=None):
    print("""<!DOCTYPE html>
    <html>
    <head>
        <title>Authentication Result</title>
        <link rel="stylesheet" href="/assets/css/accstyle.css">
    </head>
    <body>
        <div class="auth-container">
            <div class="message-box">
                <h2>%s %s</h2>
                <p>%s</p>
            </div>""" % (
        "Login" if is_login else "Registration",
        "Successful" if success else "Failed",
        message
    ))
    
    if success and is_login:
        print("""
            <div class="button-container">
                <button onclick="window.location.href='/logout.py'" class="logout-btn">
                    Logout
                </button>
            </div>
        """)
    else:
        print("""
            <div class="back-link">
                <a href="%s">%s</a>
            </div>
        """ % ('/login.html' if is_login else '/login.html',
               'Try Again' if is_login else 'Back to Login'))
    
    print("""
        </div>
    </body>
    </html>
    """)

def main():
    form = cgi.FieldStorage()
    username = form.getvalue('username')
    password = form.getvalue('password')
    firstname = form.getvalue('firstname')
    
    user_db = UserDataBase()
    
    if firstname:
        success, message = user_db.addUser(username, password, firstname)
        print_result(success, message, is_login=False)
    else:
        is_valid = user_db.validate_user(username, password)
        if is_valid:
            firstname = user_db.get_firstname(username)
            message = f"Welcome, {firstname}!" if firstname else f"Welcome, {username}!"
        else:
            message = "Invalid username or password."
        print_result(is_valid, message, is_login=True, username=username)

if __name__ == "__main__":
    main()