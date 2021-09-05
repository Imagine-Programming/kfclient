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

## Dependencies
| entity       	| dependencies                                     	| versions            	|
|--------------	|--------------------------------------------------	|---------------------	|
| all          	| libboost_system                                  	| 1.45                	|
|              	| Threads::Threads                                 	| (pthread preferred) 	|
| kfclient-cli 	| [fmt](https://github.com/fmtlib/fmt)             	| any recent version  	|
|              	| [CLI11](https://github.com/CLIUtils/CLI11)       	| any recent version  	|
|              	| [libfort](https://github.com/seleznevae/libfort) 	| any recent version  	|
| lkfclient    	| Lua 5.3 (liblua5.3-dev lua5.3)                   	| 5.3                 	|

The boost and fmt dependencies can be installed on many debian based systems, however
all of these dependencies can be built and installed as per their own instructions by
using their shipped CMakeLists with the exception of Lua. Lua 5.3 can be installed on
debian systems, but can be built from source [available at lua.org](https://lua.org).

## Building
When all dependencies are available on the build system, building all entities is really
straightforward. Create a build directory anywhere on the system and `cd` into it. Then 
invoke cmake:

```bash
cmake -DCMAKE_BUILD_TYPE=Release [path_to_kfclient_repository]
cmake --build . --config Release
```
## Installing
Using cmake, this should also be very straightforward. In the same directory that was used
to build kfclient in:

```bash
sudo cmake --install .
```

## Using the kfclient-cli application
Assuming it was built and installed, using the kfclient-cli application is quite simple.
There are not many options to choose from, when you view `kfclient --help` you'll get the
following output:

```
a very simple command line utility that implements libkfclient for obtaining information about Killing Floor 2 servers
Usage: kfclient [OPTIONS] host [port]

Positionals:
  host TEXT REQUIRED          the host of the Killing Floor 2 server to connect to.
  port UINT=27015             the port on [host] on which the Killing Floor 2 server can be polled for information. By default, this is 27015

Options:
  -h,--help                   Print this help message and exit
  -H,--help-all               show all help.
  -V,--verbose                output more information.
  -c,--clean-tables           output tables without borders.
  -P,--player-count           output the player count and nothing else
  -r,--report TEXT:{details,rules,players,d,r,p} ...
                              report a category of information (details, rules, players)
  -t,--timeout UINT=10        the timeout for datagram operations.
  -v,--version                display the version of kfclient.
``` 

**Warning:** the timeout option has not been implemented yet.

### Dumping server information
If you would like to display the details, rules and players on a server `localhost` at the default port `27015`:
```bash
kfclient -rd -rr -rp localhost
```

Or without formatted tables:
```bash
kfclient -c -rd -rr -rp localhost 
```

Or you want to get the player count on server `localhost` at port `27016`:
```bash
kfclient -P localhost 27016 | cut -d ':' -f2 | tr -d ' '
```

## Using the Lua library
Upon a successful build and install, a Lua library interfacing with libkfclient is also
installed to the system. The path is `/usr/local/lib/lua/5.3/kfclient.so`. You might have
to reconfigure `ldd` or specify `LD_LIBRARY_PATH` to include this directory on your to 
ensure that Lua can find it. 

### Simple example:
```lua
local kfc = require "kfclient";
local client = kfc.open("localhost", 27015);  -- use pcall to catch errors 
local tbl_details = client:details();         -- associative table
local tbl_rules = client:rules();             -- associative table
local tbl_players = client:players();         -- sequential table of associative tables 
client:close();                               -- terminate connection

for name, value in pairs(tbl_details) do 
  print(name, value);
end 

for rule, value in pairs(tbl_rules) do 
  print(rule, type(value), value);
end 

for _, player in ipairs(tbl_players) do 
  print(player.id, player.name, player.score, player.time);
end 
```
