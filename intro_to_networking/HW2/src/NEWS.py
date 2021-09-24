
import socket
import sctp
import os
import time
import math
import threading
import struct


# server port
port = 9000


# for the experimental purposes, read the
# whole file to the memory at once so we
# can measure the delay more accurate.

data = []
with open("input.txt", 'rb') as file:
    data = file.read()


l = len(data) # length of file in bytes
p = 500  #packet size
numberOfPackets = int(math.ceil(float(l)/p))





class RDTSender():
    def __init__(self, sock, ip, port, data, start, end):
        self.N = 200   # Window Size
        self.base = 0  # Base Number
        self.nextseqnumber = 0 # Next sequence number available

        self.acks = [False] * self.N # To see which packets are acked in the window
        self.T = 0.125 # Timeout in seconds

        self.sock = sock
        self.ip = ip
        self.port = port
        self.data = data
        self.start = start
        self.end = end

        self.times = [0] *self.N  # Timers of the packets
        self.timeLock = threading.Lock()
        self.flag = True

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
        chksum = struct.unpack('H', packet[(7+len):(9+len)])[0]
        return seq, data

    # Returns the received ack's
    # sequence number
    def parse_ackpkt(self, packet):
        seq = struct.unpack('i', packet[1:5])[0]
        chksum = struct.unpack('H', packet[5:7])[0]
        return seq

    # check if a packet is corrupt or not
    def isCorrupt(self, packet):
   	l = len(packet)
        until_chksum = packet[0:(l - 2)]
        chksum = struct.unpack('H', packet[(l - 2):(l)])[0]
        return self.checksum(until_chksum) !=  chksum


    # check if there is a timeout for
    # all the packets in the window
    def expire(self):
        while(self.flag):
            t = time.time()
            for i in range(len(self.times)):
                if(self.times[i] < t and self.times[i] > 0):
                   print "TIMEOUT " + str(self.base + i)
                   packet = self.make_datapkt(self.base + i, self.data[((self.base + i)*p):((self.base + i+1)*p)])
                   self.sock.sendto(packet, (self.ip, self.port))
                   self.timeLock.acquire()
                   self.times[i] = time.time() + self.T
                   self.timeLock.release()
            time.sleep(0.01)

    # send the data in a sequential manner
    def rdt_send(self):
        for i in range(self.start, self.end):
            message = self.data[(i*p):((i+1)*p)]
            while True:
                if(self.nextseqnumber < self.base + self.N):
                    print "SENDING " + str(self.nextseqnumber)
                    packet = self.make_datapkt(self.nextseqnumber, message)
                    self.sock.sendto(packet, (self.ip, self.port))
                    self.timeLock.acquire()
                    self.times[self.nextseqnumber - self.base] = time.time() + self.T
                    self.timeLock.release()
                    self.nextseqnumber += 1
                    break
        print "send q"

    # receive acks for the sent packets
    def rdt_rec(self):
        while(self.base < 10000):
            try:
                msg, addr = self.sock.recvfrom(10)
                if(not(self.isCorrupt(msg))):
                    seq = self.parse_ackpkt(msg)
                    print "ACK RECEIVED "+ str(seq)
                    if(seq >= self.base and seq < self.base + self.N):
                        self.acks[seq - self.base] = True
                        self.timeLock.acquire()
                        self.times[seq - self.base] = 0
                        self.timeLock.release()
                    if(seq == self.base):
                        c = 0
                        while(c < len(self.acks) and self.acks[c]):
                            c+=1
                        self.base += c
                        self.acks = self.acks[c:] + c* [False]
                        self.timeLock.acquire()
                        self.times = self.times[c:] + c*[0]
                        self.timeLock.release()
            except socket.timeout:
                print "SOCKET TIMEOUT, CLOSING THE RECEIVER"
                break
        print "rec q"
        self.flag=False

    def run(self):
        t1 = threading.Thread(target = self.rdt_send)
        t2 = threading.Thread(target = self.rdt_rec)
        t3 = threading.Thread(target = self.expire)
        t2.start()
        t3.start()
        t1.start()
        t1.join()
        t2.join()
        t3.join()


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("10.10.3.1", 0))
sock.settimeout(3) # we use this to close the connection

for i in range(1):
    x = RDTSender(sock, "10.10.4.2", 9000, data, 0, numberOfPackets)
    x.run()
    time.sleep(5)
