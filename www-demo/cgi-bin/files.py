#!/usr/bin/python3

import cgi
import os

# Pasta de uploads
UPLOAD_DIR = os.path.join(os.getcwd(), 'www-demo/uploads') # Precisa corrigir esse os.getcwd!!!!!!!!!

print(f"Debug: Upload Directory = {UPLOAD_DIR}")


# Obtem lista de arquivos do diretório de uploads
def get_uploaded_files_list(directory):
    return [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]

# Gera links HTML para os arquivos
def generate_links(files_list, base_path='/uploads'):
    links = []
    for file in files_list:
        web_filepath = os.path.join(base_path, file)
        delete_link = f'<a href="#" onclick="deleteFile(\'{web_filepath}\');">[DELETE]</a>'
        links.append(f'<a href="{web_filepath}" target="_blank">{file}</a> {delete_link}<br>')
    return ''.join(links)



files_list = get_uploaded_files_list(UPLOAD_DIR)

if not files_list:
    debug_message = f"No files found in directory: {UPLOAD_DIR}"
else:
    debug_message = f"Found {len(files_list)} files in directory: {UPLOAD_DIR}"

files_links = generate_links(files_list)


# Resposta ao cliente
print("Content-Type: text/html\n")
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Mono&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="/style.css">
    <title>w3b53rb3ru5 - Uploaded Files</title>
    <script>
    function deleteFile(filePath) {{
        if (confirm('Are you sure?')) {{
            fetch(filePath, {{ method: 'DELETE' }})
                .then(response => {{
                    if (response.status == 204) {{
                        alert('File deleted successfully');
                        location.reload();
                    }} else {{
                        alert('Error deleting file');
						location.reload();
                    }}
                }});
        }}
    }}
    </script>
</head>
<body>
    <header>
        <div class="container">
            <h1>w3b53rb3ru5 - Uploaded Files</h1>
        </div>
    </header>
    <section class="result">
        <div class="container container--center">
            {files_links}
        </div>
    </section>
    <footer>
        <div class="container">
            w3b53rb3ru5 by <a href="https://github.com/gabrielclcardoso" target="_blank" rel="noopener noreferrer">gcorreia</a> and <a href="https://github.com/lucasfads" target="_blank" rel="noopener noreferrer">lucafern</a> at 42 Rio
        </div>
    </footer>
</body>
</html>
""")

