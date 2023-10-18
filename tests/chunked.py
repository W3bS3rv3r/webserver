#!/usr/bin/python3

import socket
import time

def send_chunk(chunk):
    client_socket.sendall(chunk.encode('utf-8'))
    print("sent: <" + chunk + ">");
    time.sleep(0.5)


# Server address and port
server_address = ('localhost', 4343)

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to the server
    client_socket.connect(server_address)
    print('Connected to server.')
    message = "GET / HTTP/1.1\r\nHost: localhost:4343\r\nTransfer-Encoding: chunked\r\n\r\n"
    client_socket.sendall(message.encode('utf-8'))
    time.sleep(1)
    send_chunk("5\r\nchunk\r\n")
    send_chunk("E\r\n1/2half")
    send_chunk("2/2half\r\n")
    send_chunk("b\r\nfinal-chunk\r\n")
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
