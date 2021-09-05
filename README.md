# kfclient
A simple boost::asio based Killing Floor 2 server query protocol client

## libkfclient
This is the main library in the project providing functionality for all the interaction 
with the Killing Floor 2 Query Port protocol (usually at port 27015). This library will
use boost::asio to connect to the desired Killing Floor 2 server's query service and it
will allow the user of the library to fetch server details, rules and player lists. The
library was developed for a private project regarding KF2 server management. 

## kfclient-cli
This is the commandline utility that exposes the libkfclient API to the terminal. This 
simple tool can be used to obtain a player count or display the details, rules and the 
players currently on a server. This tool is useful to schedule server updates at times
when there are no players currently online. This way, we can schedule a daily update 
without disrupting playtime.

## lkfclient
This is the Lua 5.3 binding to libkfclient, providing all the functionality from the 
library to a Lua environment. The CMakeLists.txt and source code in this subdir can 
be easily modified to support Lua 5.4 if needed. 