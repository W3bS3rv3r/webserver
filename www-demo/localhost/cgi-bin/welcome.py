#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

user = form.getvalue('user')
title = form.getvalue('title')

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
	<title>w3b53rb3ru5 - Welcome</title>
</head>
<body>
	<header>
		<div class="container">
			<h1>w3b53rb3ru5 - Welcome</h1>
		</div>
	</header>
	<section class="result">
		<div class="container container--center">
			<p>Welcome {} - {}</p>
		</div>
	</section>
	<footer>
		<div class="container">
			w3b53rb3ru5 by <a href="https://github.com/gabrielclcardoso" target="_blank" rel="noopener noreferrer">gcorreia</a> and <a href="https://github.com/lucasfads" target="_blank" rel="noopener noreferrer">lucafern</a> at 42 Rio
		</div>
	</footer>
</body>
</html>
""".format(user, title))
