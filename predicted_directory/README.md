# README

Copyright Mark Claypool and WPI, 2018.

This is a "Hello, World" networking program for Dragonfly.


## Architecture.

It uses a classic "authoritative server" implementation.  The server's
game state is the final word.

Only the Server spawns game objects.

The Client gathers user input but doesn't act upon it. Instead, it
sends all user input to the Server.


## To Run

Build, making sure to link in Dragonfly version 4.5 (or later).

Start up Server (no command line args):

> game (.\game.exe on Windows)

Start up Client (one command line arg - the server name):

> game [server name]  (e.g., game localhost)

Note: if running from inside Visual Studio, command line
arguments can be set in the Solution properties.


## Flow

+ main() creates Server.
+ When Client connects, Server creates Reticle.
+ When Client moves/clicks mouse, sends input to Server.
+ Server receives input:
    - If move, update Reticle with EventMouseNetwork position.
    - If click, creates Explosion.
+ Server synchronizes Reticle and Explosions with Client.
    - Reticle updated upon create and move.
	- Explosions updated upon create.
+ Reticles are color-coded based on Client.
    - Do custom serialization of client socket.


## Final Word

Happy networking!

-- Mark, claypool@cs.wpi.edu
