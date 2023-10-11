#!/usr/bin/python3

import cgi
import os

form = cgi.FieldStorage()

user = form.getvalue('user')
title = form.getvalue('title')

# Response to client
print("Content-Type: text/html\n")
print("<html>")
print("<body>")
print("<p>Welcome", user, " - ", title, "</p>")
print("</body>")
print("</html>")