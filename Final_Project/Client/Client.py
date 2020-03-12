
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 62000
toSend = 'TO: ID=1'
Len = len(toSend)
header = chr(1)+Len.to_bytes(2,'big').decode()
MESSAGE = header + toSend
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

s.send(MESSAGE.encode())
while(True):
    a= 1
socket.close(s)
