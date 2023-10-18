#!/usr/bin/python3

import cgi
import os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Define a variable to store the upload message
upload_message = ''

# Define a variable to store the image tag (if the uploaded file is an image)
img_tag = ''

# Define a variable to store the file link
file_link = ''

# Test if the file was uploaded
if fileitem.filename:
    # Create full path
    filepath = os.path.join(os.getcwd(), os.path.basename(fileitem.filename))

    # Write the uploaded file
    open(filepath, 'wb').write(fileitem.file.read())
    
    # Make file name clickable
    web_filepath = f'/uploads/{os.path.basename(fileitem.filename)}'
    file_link = f'<a href="{web_filepath}" target="_blank">{os.path.basename(fileitem.filename)}</a>'
    upload_message = f'The file {file_link} was uploaded successfully!'

    # Check if the uploaded file is an image
    if fileitem.filename.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp')):
        img_tag = f'<img src="{web_filepath}" alt="Uploaded Image">'

else:
    upload_message = 'Uploading Failed'

# Response to client
print("Content-Type: text/html\n")
print("""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="preconnect" href="https://fonts.googleapis.com">
	<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
	<link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Mono&display=swap" rel="stylesheet">
	<link rel="stylesheet" href="/style.css">
	<title>w3b53rb3ru5 - Upload Result</title>
</head>
<body>
	<header>
		<div class="container">
			<h1>w3b53rb3ru5 - Upload Result</h1>
		</div>
	</header>
	<section class="result">
		<div class="container container--center">
			<p>{}</p>
			{}
		</div>
	</section>
	<footer>
		<div class="container">
			w3b53rb3ru5 by <a href="https://github.com/gabrielclcardoso" target="_blank" rel="noopener noreferrer">gcorreia</a> and <a href="https://github.com/lucasfads" target="_blank" rel="noopener noreferrer">lucafern</a> at 42 Rio
		</div>
	</footer>
</body>
</html>
""".format(upload_message, img_tag))
