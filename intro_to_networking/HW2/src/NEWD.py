import socket
import sctp
import threading
import time
import struct


## server port and  IP
port = 9000
server_ips = ["10.10.4.2"]


#packet size in bytes
p = 500


data = []
with open("input.txt", 'rb') as file:
    data = file.read()



def openSock(ip, port):
    sk = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sk.bind((ip, port))
    return sk


class RDTReceiver():
    def __init__(self, sock):
        self.N = 200 # Window size
        self.base = 0 # base number
        self.datas = "" # received data
        self.acks = [False] * self.N # to see which acks have been sent in the window
        self.buffer = [""] * self.N  # buffer to receive data out of order
        self.sock = sock
        self.startTime = False

    # UDP checksum given in the textbook
    def checksum(self, packet):
        res = 0;
        for x in packet:
            res += ord(x)
        res = res & 0xFFFF
        return res

    # 1 Byte, 'D' flag
    # 4 Byte, sequence Number
    # 2 Byte, length of the data
    # N Byte, data
    # 2 Byte, checksum
    def make_datapkt(self, seq, data):
        p = struct.pack('c', 'D') + struct.pack('i', seq) + struct.pack('h', len(data)) + data
        return p + struct.pack('H', self.checksum(p))

    # 1 Byte, 'A' flag
    # 4 Byte, sequence Number
    # 2 Byte, checksum
    def make_ackpkt(self, seq):
        p = struct.pack('c', 'A') + struct.pack('i', seq)
        return p + struct.pack('H', self.checksum(p))


    # Returns the received packet's
    # sequence number and data
    def parse_datapkt(self, packet):
        seq = struct.unpack('i', packet[1:5])[0]
        len = struct.unpack('h', packet[5:7])[0]
        data = packet[7: ( 7 + len)]
        #chksum = struct.unpack('H', packet[(7+len):(9+len)])[0]
        return seq, data


    #check if a packet is corrupt or not
    def isCorrupt(self, packet):
        l = len(packet)
        until_chksum = packet[0:(l - 2)]
        chksum = struct.unpack('H', packet[(l - 2):(l)])[0]
        return self.checksum(until_chksum) !=  chksum


    # receive data from the sender
    def rdt_rec(self):
        while(self.base < 10000):
            msg, addr = self.sock.recvfrom(509)
            if(self.startTime == False):
                self.startTime = time.time()
            if(self.isCorrupt(msg)):
                continue

            seq, data = self.parse_datapkt(msg)
            print "RECEIVED " + str(seq)
            if(self.base <= seq and seq < self.base + self.N):
                self.buffer[seq - self.base] = data
                self.acks[seq - self.base] = True
                ack = self.make_ackpkt(seq)
                self.sock.sendto(ack, addr)
                print "ACK SENT " + str(seq)
                if(self.base == seq):
                    c = 0
                    while(c < self.N and self.acks[c]):
                        self.datas = self.datas + self.buffer[c]
                        c+=1
                    self.base += c
                    self.acks = self.acks[c:] + c*[False]
                    self.buffer = self.buffer[c:] + c*[""]
            elif(self.base - self.N <= seq and seq < self.base):
                ack = self.make_ackpkt(seq)
                self.sock.sendto(ack,addr)
                print "ACK SENT " + str(seq)



sock = openSock(server_ips[0], port)


for i in range(1):
    x = RDTReceiver(sock)
    x.rdt_rec()
    t = time.time() - x.startTime
    print t
   # print x.datas == data
    with open("outputSON", "ab") as file:
        file.write(x.datas)
    time.sleep(4)
