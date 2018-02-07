#Dev Plan M1_Spacewar Multiplayer

* Server needs a socket channel for each client
* Server needs a static port where the clients can connect to
* Clients have to be allowed in the Windows Firewall
* Clients bind random ports
* UDP packet size should not be beyond 1500 Byte

1. Clients open ports in Windows Firewall
2. Server listens to his port and waits for 2 Clients to connect. wait for 2 unique endpoints 
3. Send the game start time to the clients
4. Create a thread to receive and a thread to send for each client