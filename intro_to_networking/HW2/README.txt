MURAT TOPAK 2036218

------------ROUTING----------
-----------------------------
-----------------------------


Initially, one interface of the source was connected to the destination.
But the other one needed route commands on the source and destination.

On the source, the entry with 10.10.2.2 has been deleted and the following command was run:

> sudo route add -net 10.10.2.0 netmask 255.255.255.0 gw 10.10.1.2 dev eth1


On the destination, the entry with 10.10.3.2 has been deleted and the following command was run:

> sudo route add -net 10.10.3.0 netmask 255.255.255.0 gw 10.10.4.1 dev eth1


Here is the route -n output for the source:

e2036218@s:~$ route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         172.16.0.1      0.0.0.0         UG    0      0        0 eth0
10.0.0.0        10.10.3.2       255.0.0.0       UG    0      0        0 eth2
10.10.1.0       0.0.0.0         255.255.255.0   U     0      0        0 eth1
10.10.2.0       10.10.1.2       255.255.255.0   UG    0      0        0 eth1
10.10.3.0       0.0.0.0         255.255.255.0   U     0      0        0 eth2
172.16.0.0      0.0.0.0         255.240.0.0     U     0      0        0 eth0

For the destination:

e2036218@d:~$ route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         172.16.0.1      0.0.0.0         UG    0      0        0 eth0
10.0.0.0        10.10.2.1       255.0.0.0       UG    0      0        0 eth2
10.10.2.0       0.0.0.0         255.255.255.0   U     0      0        0 eth2
10.10.3.0       10.10.4.1       255.255.255.0   UG    0      0        0 eth1
10.10.4.0       0.0.0.0         255.255.255.0   U     0      0        0 eth1
172.16.0.0      0.0.0.0         255.240.0.0     U     0      0        0 eth0


No modifications has been made to routers so they are not here.
On the source, we have pinged and run traceroute to test connectivity.

e2036218@s:~$ sudo traceroute 10.10.4.2 -i eth2
traceroute to 10.10.4.2 (10.10.4.2), 30 hops max, 60 byte packets
 1  r2-link-1 (10.10.3.2)  6.593 ms  6.572 ms  6.549 ms
 2  d-link-3 (10.10.4.2)  13.329 ms  13.315 ms  13.292 ms

e2036218@s:~$ sudo traceroute 10.10.2.2 -i eth1
traceroute to 10.10.2.2 (10.10.2.2), 30 hops max, 60 byte packets
 1  r1-link-0 (10.10.1.2)  6.613 ms  6.592 ms  6.571 ms
 2  d-link-2 (10.10.2.2)  13.076 ms  13.062 ms  13.041 ms

Hence, no problems on the routing side.



----------LIBRARIES--------
-----------------------------
-----------------------------

PySCTP has been used throughout the project.
https://github.com/philpraxis/pysctp

To get it working, first we have run the updates on the Ubuntu

> sudo apt-get update

Then followed the instructions on the github.

> sudo make installdeps

> sudo python setup.py install

We are now able to import sctp.


----------RUN THE CODES------
-----------------------------
-----------------------------
In all of the situations,
first run the dst, and then src.

-----------SCTP-------------
----------------------------

Related files for SCTP: "s.py", "d.py".
                         |src|   |dst|
  
In those, you will find the line

"r = 2 # number of links available"

You can make this variable 1 or 2.
1 means no multihoming, and 2 means multihomed.
Please make sure the two files have the same "r" value.

At the end of those files, there is a loop to run the code
like the following:

for _ in range(1):

By making it run only once, you can verify
the output.txt created by the d.py does not differ from input.txt

Small note: There must be the same number of iterations so
any change in one of the files regarding the range
of the loop must be made in the other too.


-----------RDT-------------
----------------------------

Related files: "NEWS.py", "NEWD.py"
                 |src|      |dst|

These files are for the single link experiments,
in which we are asked to implement an RDT.

Again, there is a loop at the end:

for i in range(1):

Please make sure the two files have the same range.


--------Multihomed RDT------
----------------------------

Related files: "NEWSTHREAD.py", "NEWDTHREAD.py"
                   |src|            |dst|

The configuration is the same as above.


--------Verification of RDT------
---------------------------------
To verify the same file arrives to the destination
you may run the loops only once. 
This is already the configuration. The created file at the 
server side will be named as "outputSON".

Also, I have commented out the following lines in NEWD.py and NEWDTHREAD.py, respectively.

# print x.datas == data
# print data == (x.datas + y.datas)

You may uncomment them to see if they return True to the terminal.


--------ASSUMPTIONS-------------
---------------------------------
There is a file named "input.txt" residing on where the source script runs.


--------MEASUREMENT--------------
---------------------------------

NTP has not been used becaue our measurement happens only at the server side.
Once a connection is accepted we start to count, when it finishes we note the time passed.