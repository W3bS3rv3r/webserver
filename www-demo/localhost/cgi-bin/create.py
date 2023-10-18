#!/usr/bin/env python
import cgi
import os
import sys

# Read the raw POST data
post_data = ""
while True:
    line = sys.stdin.readline()
    if not line:
        break
    post_data += line

# Write the POST data to a file
with open(os.path.join("./", 'uploaded_data.txt'), 'w') as f:
    f.write(post_data)

print("Content-Type: text/plain\n")
print("POST data saved successfully.")