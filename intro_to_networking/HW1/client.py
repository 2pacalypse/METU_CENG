#CLIENT U1
from socket import *
import sys
import thread
from multiprocessing.dummy import Pool as ThreadPool
import struct
import time
from time import sleep

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
should be used between two ends
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

"""
These ports are used throughout
all the files.
"""        
tcpPort = 9000
udpPort = 9001


  

#serverSocket = socket(AF_INET, SOCK_DGRAM)
#serverSocket.bind((interfaces[1][0], udpPort))
#serverSocket.setTimeout(10)
#print "Listening(UDP)" + interfaces[1][0] + ":" + str(udpPort)


"""
creates a message
with the time information of the creation.
"""
def createMsg(frm, to, msg):
    return str(frm) + str(to) + struct.pack("d", time.time()) +  msg

"""
parses a message
"""
def parseMsg(msg):
    return int(msg[0]), int(msg[1]), struct.unpack("d", msg[2:10]), msg[10:]




"""
get the destination from the command line
turn it into our index system
"""
DESTINATION = sys.argv[1]
if(DESTINATION == "U3"):
    DESTINATION = 5
elif(DESTINATION == "T3"):
    DESTINATION = 6


if(node == 1): #experiments 3,4
    """
    send 10k packets to identify
    the packet losses. 
    """
    for i in range(10000):
        sampleMsg = createMsg(1, DESTINATION,str( i))
        outsock = socket(AF_INET, SOCK_DGRAM)
        outsock.sendto(sampleMsg, (interfaces[0][1], udpPort))
        outsock.close()
        sleep(0.3)
        
elif(node == 2): #experiments 1,2
    """ 
    listen on TCP to get acknowledgment from server
    and see end to end delays.
    """ 
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.settimeout(30)
    serverSocket.bind((interfaces[2][0], tcpPort))
    serverSocket.listen(1)
    print "Listening(TCP)" + interfaces[2][0] + ":" + str(tcpPort)
    
    """
    send 100 packets for e2e.
    """

    for _ in range(100):
        sampleMsg = createMsg(2, DESTINATION, "msg")
        start = time.time()
    
        outsock = socket(AF_INET, SOCK_STREAM)
        outsock.connect((interfaces[0][2], tcpPort))
        outsock.send(sampleMsg)
        outsock.close()

        connect, addr = serverSocket.accept()
        recv = connect.recv(1024)
        end = time.time()
        connect.close()
        src, dst, t, msg = parseMsg(recv)
        e2e = ( end - t[0])*1000
        print str(e2e)
    