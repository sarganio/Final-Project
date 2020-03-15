
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = [62000,62001,62002]
sockets =[] 

toID = input('To:')

for i in range(3):
    sockets.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
TCP_PORT = TCP_PORT[int(toID)]
s = sockets[int(toID)]
s.connect((TCP_IP, TCP_PORT))
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((TCP_IP, TCP_PORT))



while(True):
    type = input('Insert type of message:')
    toSend = toID+': '+type
    Len = len(toSend) 
    header = type + Len.to_bytes(2,'big').decode()
    MESSAGE = header + toSend
    s.send(MESSAGE.encode())
socket.close(s)
