import socket
import sctp
import os
import time
import math
import threading


# server IPs
# server Port
dest_ips = ["10.10.4.2", "10.10.2.2"]
port = 9005


# for the experimental purposes, read the
# whole file to the memory at once so we
# can measure the delay more accurate.
data = []
with open("input.txt", 'rb') as file:
    data = file.read()



### YOU CAN MAKE r = 1, or r = 2
### BU BE SURE s.py and d.py HAS
### THE SAME r value

r = 2 # number of links available.
l = len(data) # length of file in bytes
p = 500 #packet size
numberOfPackets = int(math.ceil(float(l)/p))
step = int(math.ceil(numberOfPackets/float(r))) # [0..step) U .. U .. U  [step..numberOfPackets) are to be sent


# there are L - 1 intervals in a list of length "L"
# e.g [0,2500,5000]
# I_1 = [0,2500)
# I_2 = [2500,5000)
# Interval I_n denotes the packets that are to be sent to dest_ips[n].
intervals = []
for i in range(0,numberOfPackets, step):
    intervals.append(i)
intervals.append(numberOfPackets)


#Dest IP, Dest Port, data, start packet number, endpacke number
def sctpsend(ip, port, data, start, end):
    sock = sctp.sctpsocket_tcp(socket.AF_INET) ###OPEN A SCTP SOCKET
    sock.connect((ip, port))
    for i in range(start, end):
        print "SENDING "+ str(i) + "/" + str(end)
        sock.sendall(data[(i*p):((i+1)*p)]) ###SENDALL IS MORE RELIABLE
    sock.close()



### YOU CAN CHANGE THE range(1)
### TO REPEAT THE EXPERIMENT
### MORE THAN ONCE

for _ in range(1):
    threads = []
    for i in range(r):
        t1 = threading.Thread(target = sctpsend, args = (dest_ips[i], port, data, intervals[i], intervals[i+1]))
        threads.append(t1)

    for t in threads:
        t.start()

    for t in threads:
        t.join()

print "FINISHED"
