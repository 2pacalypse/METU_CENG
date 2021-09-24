from socket import *
import sys
import thread
from multiprocessing.dummy import Pool as ThreadPool
import struct
import time


"""
First of all each node is assigned and ID
so that the same code can be used on different
machines with changing the 'node = x' line below
U1, U2, U3 -> 1, 3, 5
T1, T2, T3 -> 2, 4, 6
Gateway -> 0
"""
node = 1


"""
The following is a dictonary of dictionaries and intended
to use for listening the interfaces.
interfaces[x][y] is x's data interface in the link x-y.
Note that direction is important so interfaces[x][y]
and interfaces[y][x] both refer to the same link 
but to different ends.
"""
interfaces = {
     0: {1: "10.10.2.2",
         2: "10.10.1.2",
         3: "10.10.4.1",
         4: "10.10.3.1"},
              
     1: {0: "10.10.2.1"},
     
     2: {0: "10.10.1.1"}, 
     
     3: {0: "10.10.4.2",
         5: "10.10.6.1"},
              
     4: {0: "10.10.3.2",
         6: "10.10.5.1"},
              
     5: {3: "10.10.6.2"},
     6: {4: "10.10.5.2"}        
              }

"""
Again, a dictionary of dictionaries for fast access,
this routing table gives information about which node to go next
when it is either on Gateway(0), or on the routers T2(4), U2(3).

routing_table[x][y] gives the next node when x is the 
current node and y is the ultimate destination.
"""
routing_table = {
    0: {
        1: 1,
        2: 2,
        3: 3, 
        4: 4, 
        5: 3, 
        6: 4},
    3:{
       0: 0,
       1: 0,
       2: 0,
       5: 5
       },
    4: {
        0: 0,
        1: 0,
        2: 0,
        6: 6
        }

         }

"""
This table decides whether UDP or TCP 
should be used between two links
if links[x][y] == 0 -> UDP
otherwise -> TCP

"""
links = {
     0: {1: 0,
         2: 1,
         3: 0,
         4: 1},
              
     1: {0: 0},
     
     2: {0: 1}, 
     
     3: {0: 0,
         5: 0},
              
     4: {0: 1,
         6: 1},
              
     5: {3: 0},
     6: {4: 1}        
              }        

tcpPort = 9000
udpPort = 9001

"""
Simple function to open a TCP socket with
the specified IP and port.
"""
def tcp_server(ip, port):
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.bind((ip, port))
    serverSocket.listen(1)
    #print "Listening(TCP)" + ip + ":" + str(port)
    return serverSocket

"""
Simple function to open a UDP socket with
 the specified IP and port
"""
def udp_server(ip, port):
    serverSocket = socket(AF_INET, SOCK_DGRAM)
    serverSocket.bind((ip, port))
    #print "Listening(UDP)" + ip + ":" + str(port)
    return serverSocket

"""
Given an opened TCP socket,
this method listens to it
and tries to send the message
to the next node decided from
the routing table
"""
def tcp_listen(socketT):
    while 1:
        connectionSocket, addr = socketT.accept()
        message = connectionSocket.recv(1024)
        src, dst, a , msg = parseMsg(message)
        if(src == 1):
            print msg
        else:
            server = interfaces[4][node]
            sock = socket(AF_INET, SOCK_STREAM)
            feedback = str(dst) + str(src) + struct.pack("d", time.time()) + msg
            sock.connect((server, tcpPort))
            sock.send(feedback)
            sock.close()
        
        connectionSocket.close()

"""
Given an opened UDP socket,
this method listens to it
and tries to send the message
to the next node decided from
the routing table
"""        
def udp_listen(sockett):
    while 1:
        message, clientAddress = sockett.recvfrom(2048)
        src, (dst),_, msg = parseMsg(message)
        if(src == 1):
            print msg
        else:
            server = interfaces[3][node]
            sock = socket(AF_INET, SOCK_DGRAM)
            sock.sendto(str(dst) + str(src) + struct.pack("d", time.time()) + msg, (server, udpPort))
            sock.close()

"""
creates a message
with the time information of the creation.
"""
def createMsg(frm, to, msg):
    return str(frm) + (to) + struct.pack("d", time.time()) +  msg

"""
parses a message
"""
def parseMsg(msg):
    return int(msg[0]), int(msg[1]), struct.unpack("d", msg[2:10]), msg[10:]


"""
simple method to send a message to the destination
over UDP
""" 
def sendMsgUDP(msg, dst):
    #print "Sending on UDP to " + str(dst)
    server = interfaces[dst][node]
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.sendto(msg, (server, udpPort))
    sock.close()
    
"""
simple method to send a message to the destination
over TCP
"""     
def sendMsgTCP(msg, dst):
    #print "Sending on TCP to " + str(dst)
    server = interfaces[dst][node]
    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect((server, tcpPort))
    sock.send(msg)
    sock.close()
    

"""
arrays to be used for multi-threading
"""
tcpSocks = []
udpSocks = []
"""
decide whether to open a TCP or UDP sock
between node and othernode, node having the ip.
This method keeps track of the opened sockets
by putting them into the respective arrays above.
It can be optimized but,
'if it ain't broke, don't fix it' :)
"""
def opensock(node, othernode, ip):
    
    if(node == 0):
        if(othernode % 2 == 0):
            tcpSocks.append(tcp_server(ip, tcpPort))
        else:
            udpSocks.append(udp_server(ip, udpPort))
    else:
        if(othernode == 0):
            if(node % 2 == 0):
                tcpSocks.append(tcp_server(ip, tcpPort))
            else:
                udpSocks.append(udp_server(ip, udpPort))
        else:
            if(node % 2 == 0):
                tcpSocks.append(tcp_server(ip, tcpPort))
            else:
                udpSocks.append(udp_server(ip, udpPort))

# LISTEN THE INTERFACES ON THE NODE
"""
Remember node is the variable indicating
the current machine the script runs on.
the following line finds the all interface IPs
that are to be opened for listening
"""
item_list = interfaces.get(node).items()


"""
the above for loop fills the
tcpSocks, and udpSocks arrays
Each socket should be listened on
its own thread so that everything works fine
"""
for item in item_list:
    opensock(node, item[0], item[1]) 
        
for sock in tcpSocks:
    thread.start_new_thread(tcp_listen, (sock, ))

for sock in udpSocks:
    thread.start_new_thread(udp_listen, (sock, )) 
    
while 1:
    pass   
            
