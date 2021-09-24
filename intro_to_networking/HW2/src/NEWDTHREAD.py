import socket
import sctp
import threading
import time
import struct



port = 9000
server_ips = ["10.10.4.2", "10.10.2.2"]

p = 500 # packt size in bytes

data = []
with open("input.txt", 'rb') as file:
    data = file.read()



def openSock(ip, port):
    sk = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sk.bind((ip, port))
    return sk


class RDTReceiver():
    def __init__(self, sock, end):
        self.N = 200 # Window size
        self.base = 0 # base number
        self.datas = "" # received data to give it to the upper layer
        self.acks = [False] * self.N # to see the acks we have sent
        self.buffer = [""] * self.N # to hold the data out of order
        self.sock = sock
        self.startTime = False
        self.end = end


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


    # returns the sequence number
    # and data of a packet
    def parse_datapkt(self, packet):
        seq = struct.unpack('i', packet[1:5])[0]
        len = struct.unpack('h', packet[5:7])[0]
        data = packet[7: ( 7 + len)]
       # chksum = struct.unpack('H', packet[(7+len):(9+len)])[0]
        return seq, data


    #check if a packet is corrupt or not
    def isCorrupt(self, packet):
        l = len(packet)
        until_chksum = packet[0:(l - 2)]
        chksum = struct.unpack('H', packet[(l - 2):(l)])[0]
        return self.checksum(until_chksum) !=  chksum

    # receiver thread to receive data
    def rdt_rec(self):
        while(self.base < self.end):
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

        


socks = []
for i in range(len(server_ips)):
    sock = openSock(server_ips[i], port)
    socks.append(sock)

for i in range(1):
    x = RDTReceiver(socks[0], 5000)
    y = RDTReceiver(socks[1], 5000)
    t1 = threading.Thread(target = x.rdt_rec)
    t2 = threading.Thread(target = y.rdt_rec)
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    tEnd = time.time()
    print tEnd - min([x.startTime] + [y.startTime])
   # print data == (x.datas + y.datas)
    with open("outputSON", "ab") as file:
        file.write(x.datas)
        file.write(y.datas)
    time.sleep(4)
