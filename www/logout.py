#!/usr/local/bin/python3

from http import cookies
import os

def clear_cookie():
    cookie = cookies.SimpleCookie()
    cookie['session'] = ''
    cookie['session']['path'] = '/'
    cookie['session']['max-age'] = 0
    print(cookie.output())

print("Content-Type: text/html\r\n\r\n")
clear_cookie()
print("""
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="0;url=/login.html">
</head>
<body>
    Logging out...
</body>
</html>
""")
