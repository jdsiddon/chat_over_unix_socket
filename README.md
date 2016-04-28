# Readme

### Usage Instructions
To compile run:  
`make server`
`make client`

To clean compiled code:  
`make clean`

To start the chatserve (host A) run:  
`./server [port number]`

To start the chatclient (host B) run:  
`./client [host name] [port number]`

#### chatclient
Once the client starts you will be prompted to enter your 'handle'  
`Enter username: `

When you see your handle, the client is ready for you to enter another message to send to the server.  
`[username]> `

#### chatserve
Once the server starts you will have to wait to receive a message from a client before you can send a response. You will be prompted to enter a response when you see this:  
`localhost> `



### Sources
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
http://www.linuxhowtos.org/C_C++/socket.htm
http://www.thegeekstuff.com/2012/03/c-process-control-functions/
http://www.yolinux.com/TUTORIALS/Sockets.html




### Test Input
Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages.
