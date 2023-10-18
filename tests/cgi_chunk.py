#!/usr/bin/python3

import socket
import time

def send_chunk(chunk):
    client_socket.sendall(chunk.encode('utf-8'))
    print("sent: <" + chunk + ">");
    time.sleep(0.5)


# Server address and port
server_address = ('localhost', 4242)

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to the server
    client_socket.connect(server_address)
    print('Connected to server.')
    message = 'POST /cgi-bin/create.py HTTP/1.1\r\n\
Host: localhost:4242\r\n\
Transfer-Encoding: chunked\r\n\r\n'
    print(message)
    client_socket.sendall(message.encode('utf-8'))
    time.sleep(1)
    send_chunk("5\r\nthis\n\r\n")
    send_chunk("12\r\nfile was ")
    send_chunk("uploaded\n\r\n")
    send_chunk("a\r\nin chunks\n\r\n")
    send_chunk("0\r\n\r\n")
    # Receive the response from the server
    response = client_socket.recv(1024).decode('utf-8')
    print('Response from server:\n',response)

except Exception as e:
    print('An error occurred:', str(e))

finally:
    # Close the socket
    client_socket.close()
    print('Socket closed.')
