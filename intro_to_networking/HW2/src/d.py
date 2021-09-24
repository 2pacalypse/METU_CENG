import socket
import sctp
import threading
import time


### server port
### server ips
port = 9005
server_ips = ["10.10.4.2", "10.10.2.2"]

p = 500 # packet size in bytes
r = 2 # number of available links


#helper method to create an SCTP socket
def openSock(ip, port):
    sk = sctp.sctpsocket_tcp(socket.AF_INET)
    sk.bind((ip, port))
    sk.listen(3)
    return sk


# given a socket,
# returns the received data with "outList"
# also reports the time just before the 
# first transmission with "startTimeList" 
def handle_sock(sock, outList, startTimeList):
    data = ""
    client, addr = sock.accept()
    startTimeList.append(time.time())
    message = client.recv(p)
    while(message):
        data = data + message
        message = client.recv(p)
    client.close()
    outList.append(data)



socks = []
for i in range(r):
    sock = openSock(server_ips[i], port)
    socks.append(sock)
### socks holds exactly r socket objects
### i.e we are gonna listen the available interfaces


for _ in range(1):
    datas = [ [], [] ]
    startTimes = [[], [] ]
    threads = []
    k = 0

    #multithreading happens here
    for i in range(r):
        t1 = threading.Thread(target = handle_sock, args = (socks[i], datas[i], startTimes[i]))
        threads.append(t1)

    for t in threads:
        t.start()

    for t in threads:
        t.join()
    #multithreading ends here

    end = time.time()
    min = startTimes[0][0]
    for m in range(r-1):
        if(startTimes[m + 1][0]  < min):
            min = startTimes[m + 1][0]
    #transmission time
    print end - min

    #transmitted file
    for b in range(r):
        with open("output.txt", "ab") as file:
            file.write(datas[b][0])

