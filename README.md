# Readme

### Usage Instructions
To compile run:  
`make server`
`make client`

To clean compiled code:  
`make clean`

To start the chatserve (host A) run:  
`./chatserve [port number]`

To start the chatclient (host B) run:  
`./chatclient [host name] [port number]`

#### chatclient
Once the client starts you will be prompted to enter your 'handle'  
`Enter username: `

When you see your handle, the client is ready for you to enter another message to send to the server.  
`[username]> `

#### chatserve
Once the server starts you will have to wait to receive a message from a client before you can send a response. You will be prompted to enter a response when you see this:  
`localhost> `  

#### Quit
To quit either the client or server enter `\quit` in the users chat message input field.
*Server will output "Killed" message when server has quitted.


### Sources
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
http://www.linuxhowtos.org/C_C++/socket.htm
http://www.thegeekstuff.com/2012/03/c-process-control-functions/
http://www.yolinux.com/TUTORIALS/Sockets.html


### Testing

#### Large test input
The following was used to test that the client/server supports up to 500 character messsages:
  Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages. Hello this is 500 characters worth of text to test if the OS breaks apart each message into multiple messages.

#### Test Systems
The software was tested on a Ubuntu 14.04.3 LTS and FLIP OSU servers.

### Troubleshooting

#### Server
`ERROR binding: Permission denied`
*Make sure the selected port isn't busy already.

#### Client
`ERROR connecting: Connection refused`  
*Make sure the specified client port is the same as the server port.
*If on the FLIP OSU servers make sure you are attempting to connect under the same SSH session (use screen or similar to create two windows under the same session).
