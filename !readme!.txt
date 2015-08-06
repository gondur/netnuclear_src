FIRST OF ALL
------------------------------------------------------------------------------
This is the FOURTH version of NetNuclear, and while it's an improvement over
the previous versions it's still not final. Like the previous versions, it's
playable, but lacks some of the features that were present in the original
game. Most importantly the numerous random events that sometimes occur during
the game, the world leaders different playing styles, the attitude settings,
and some animations. A lot of numbers and calculations, like for example how
much damage a warhead should do to a city, are also still subject to reverse
engineering and may not be entirely right in this version (either). Also, the
AI isn't exactly "intelligent", and all computer opponent actions are more or
less entirely random for now. There could also be a bug or two creeping around
in there, but I THINK I've gotten rid of all of them now :-). The features
that are still missing from the original game are ofcourse planned to be added
for a later release. Possibly the next one. Until then, I hope you enjoy some
MULTIPLAYER Nuclear War!

John David Karlgren
1 February 2010


DESCRIPTION
------------------------------------------------------------------------------
NetNuclear is a freeware TCP/IP multiplayer remake of the classic 1989
computer game Nuclear War by New World Computing, Inc. The original game was
released for Commodore Amiga and DOS, which are both obsolete systems today.
That, plus the fact that the original game was single player only, gave me the
idea of remaking the game for modern systems in portable C code using SDL for
graphics, sound and network functions, and throwing in multiplayer support for
extra fun! The game can be played against up to four opponents, which can be
computer opponents, human opponents or both at the same time.


HOW TO PLAY
------------------------------------------------------------------------------
NetNuclear is extremely simple to play. I haven't had time to write down
proper instructions yet, so in the meanwhile I'd like to refer you to the
manual for the original Nuclear War, which can be downloaded from this link:
<http://www.thehouseofgames.net/index.php?t=10&id=49>

There is however a command console in NetNuclear which I should probably take
some time to explain. First of all, to bring it up, press RETURN or ENTER on
your keyboard. You are then able to type the following commands:

/add ........... Add a computer opponent to the game, if you're the server.
/clear ......... Clear the telegraph "paper".
/close ......... Close (shutdown) the server, if you're hosting one.
/connect ....... Connect to a server. Usage: /connect <address> [port]. If no
                 port is specified, the default (1989) will be used.
/disconnect .... Disconnect from the server, if you're a client.
/help .......... List all available commands.
/host .......... Host a server. Usage: /host [port]. If no port is specified,
                 the default (1989) will be used.
/name .......... Set your player name. Usage: /name <name>.
/quit .......... Close (shutdown) the program.
/start ......... Start a new game, if you're the server.
/turn .......... Display the current turn number.
/version ....... Display the program version.
/who ........... List all players in the game, including computer opponents.

If you type a sentence without the beginning "/" that sentence will be sent to
the other players as a chat message. Chat messages are displayed on the screen
in a bubble next to the player who typed it.


CONTACT
------------------------------------------------------------------------------
If you have any questions, suggestions, praise or even complaints, please
e-mail me, the author, on <david@studiostok.se>. You may also contact me on
MSN by adding <kada@telia.com> to your contacts (NOTE: this address is unable
to receive e-mail). I am also available on Facebook under my real name.

Ultimately, check out my website <http://www.studiostok.se> for new releases
of NetNuclear and other games I'm working on.


AUTHORS
------------------------------------------------------------------------------
Nuclear War is (C) 1989 New World Computing, Inc.
Design by Eric Hyman and Jon Van Caneghem
Program by Eric Hyman
Graphics by Avril Harrison

NetNuclear is (P) 2009 John David Karlgren
A Studio Stök Release
<http://www.studiostok.se>

SDL (Simple DirectMedia Layer) by Sam Lantinga
<http://www.libsdl.org>

SDL_mixer by Sam Lantinga, Stephane Peter, and Ryan Gordon
<http://www.libsdl.org/projects/SDL_mixer/>

SDL_net by Sam Lantinga, Roy Wood and Masahiro Minami
<http://www.libsdl.org/projects/SDL_net/>


DISCLAIMER
------------------------------------------------------------------------------
NetNuclear is an unauthorized remake which is not sponsored, endorsed, sold or
promoted by New World Computing, Inc. in any way or matter. This remake is
developed and released by indie game developer Studio Stök and is meant to be
viewed as a tribute and improvement to the original Nuclear War game. If there
are any legal issues concerning this software, which I certainly hope not,
please e-mail <david@studiostok.se> for immediate discontinuation of this
software and complete removal of any related content from the website
<http://www.studiostok.se>. Thank you.
