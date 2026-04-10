_This project has been created as part of the 42 curriculum by yookyeoc, hucherea and keait-he._

---

## 📖 Description

This project consists of creating our own IRC server. The server is developed strictly adhering to the C++ 98 standard.<br> It is capable of handling multiple clients concurrently without blocking, utilizing non-blocking I/O operations and a single `poll()` (or equivalent) call to manage all socket operations.

It implements the core features of an IRC server, allowing users to authenticate, set a nickname and a username, join channels, and send or receive private messages. It also includes channel operator privileges with the commands KICK, INVITE, TOPIC, and MODE.

---

## 📚 Good to Know

* **Message Structure:** Format `<prefix> <command> <params> \r\n` (max 512 bytes).
* Consists of up to three main parts: the prefix (OPTIONAL), the command, and the parameters (maximum of 15).
* The presence of a prefix is indicated by a `:` as the first character of the message.
* There is no space between the colon and the prefix.
* The prefix is used by servers to indicate the origin of the message.
* The only valid prefix for clients is their registered nickname.
* Our IRC considers params[3] as a real name, in case of no trailing, for the better portability of client's choice.

* **Server workflow:** Developping IRC Server not an IRC client.
* Set sig -> configurating Server(socket() bind() listen()) -> loop while no signal flag
* doing : 1. set poll events flags for all clients fds 2. poll() 3. check revents and do the right actions(recv(), send(), upd to delete set) 4. disconnect according to the to delete container

---

## 🚀 Usage Instructions

### Compilation
The project uses a Makefile for compilation. You can compile the server using standard rules:
* `make` or `make all`: Compiles the main program.
* `make clean`: Removes object files.
* `make fclean`: Removes object files and the executable.
* `make re`: Fully recompiles the project.

### Execution
The executable must be launched with the following command:
`./ircserv <port> <password>`

* **port**: The port number on which the server listens for incoming connections.
* **password**: The connection password required for any client attempting to connect.

---

## 🧪 Testing Tools

We have set up two test suites to ensure our server's stability, which you can run via the Makefile:

1.  **Unit Tests (Methods):**

    Run the command `make test` to compile and execute our test binary `./irc_tests`. These tests validate the internal behavior of the client, channels, validator, and parser.

2.  **Integration Tests (Commands):**

    Run the command `make test_integration`. This Makefile rule handles:
    * Starting the IRC server in the background on port 6667 with a test password.
    * Executing our custom Python script (`test/commands_tests/run_integration.py`) which simulates client interactions.
    * Automatically shutting down the server once the tests are completed.
    * Displaying a success or error message based on the results.

---

## 🎓 Evaluation Preparation (Technical Q&A)


yookyeoc:

Q0. How team work has done?<br>

A0. We divided our work in Server / Parsing / Execution parts basically, and started doing the project by making proper classes for its own work.<br>
hucherea suggested to make a verification system on github so that we don't corrupt main branch by several push and using in different ways.<br> 
Beside, we used group chat so to ask and verify faster.

Q1. Why poll()?

A1. Looked simple by its prototype and the use of struct pollfd with BitMask flags seemed interesting.<br>
It keeps events to check not like select() and check all the fds by loop can be tedious but still the way we are used to it by 42 projects.<br>

Q2. Some features on server part?

A2-1. We use set container to delete client fds.<br>
Even though to delete checking is done several times for the same fd, as set doesnt keep the several same elements inside, it really prevents misstakes from different branch points of functions.<br>

A2-2. I also like that we set up the server fd on the poll list[0] position, and done it on the configuration part which adds lisibility and divided actions for the server and clients.<br>
* (in case of further server part details, I fully commented my part and pushed on our team git)
---

## 🤖 Bonus: Kuru_bot

As a bonus feature, we've implemented a dedicated IRC Bot that can interact with users on the server.

### Features
* **Missile Launcher:** Use `!missile` in any channel where the bot is present to trigger a random missile launch (Nuclear, Foam, Ballistic, etc.).
* **Independent Client:** The bot runs as a separate process, demonstrating the server's ability to handle automated TCP clients.

### How to use
1. **Compile the bot:**
   ```bash
   make bot
   ./irc_bot <server_ip> <port> <password>

## 📚 Resources and References

listing classic references related to the topic (documentation, articles, tutorials etc.), as well as a description of how AI was used specifying for which tasks and which parts of the project.

<keait-he's ressources:<br>
	- https://www.irc.info/articles/rfc-1459 
	- https://www.polom.fr/doc/irc/ Basic understanding of the project
	- https://www.rfc-editor.org/rfc/rfc2812#section-2.3 To know about Messages' structures
	- https://cplusplus.com/reference For functions' usage
	- https://mathieu-lemoine.developpez.com/tutoriels/irc/protocole/?page=connexion For general understanding of commands
	- https://man.openbsd.org/gethostname.3 gethostname() fct + 256 standard buffer size
	- https://dd.ircdocs.horse/refs/commands/topic topic usage
	- http://www.iprelax.fr/irc/irc_rfcfr6.php error messages numeric answer



<yookyeoc's ressources:<br>
    - https://www.linuxhowtos.org/C_C++/socket.htm sockets tutorial which helps understanding socket() bind() listen() accept() steps<br>
    - https://codingfarm.tistory.com/539 connect() post to understand client's own part<br>
    - https://codingfarm.tistory.com/538?category=812608 best explanation I found to understand differences between config functions - bind() listen() accept()<br>
    - https://en.cppreference.com/w/cpp/container/set/clear.htm clear()<br>
    - https://jacking75.github.io/linux_socket_sigpipe/ SIGPIPE<br>
    - https://velog.io/@junttang/SP-1.6-%EC%8B%9C%EA%B7%B8%EB%84%90-%ED%95%B8%EB%93%A4%EB%A7%81Signal-Handling Signal Handling<br>
    - https://man7.org/linux/man-pages/man2/ppoll.2.html linux manual pages for library, errno, purpose, and return value verifications<br>
    - https://www.rfc-editor.org/rfc/rfc1459.html RFC 1459 reference<br>
    - https://chipmaker.tistory.com/entry/%EA%B3%A0%EA%B8%89-IO-poll-%ED%95%A8%EC%88%98 poll()<br>


---

## 🤖 AI Usage Disclosure

hucherea:

	- Used AI as a complement to official documentation (RFC 2812) to clarify protocol specifics.

	- Applied AI for code review after initial development to ensure "Clean Code" standards and improve logic efficiency.

	- Researched optimal C++ 98 patterns for network programming.

yookyeoc:

    - Used AI for understanding better errno after reading man, for main functions of server part.
    
    - Learning and correcting iterator grammars.
    
    - Verifying to AI if the terms that I'm using for comments are properly understood and not wrong so to not spread wrongly understood infos to teammates.

keait-he:

	- In accordance with 42 project directives, AI was used to assist in:

		-> Clarifying some RFC concepts.

		-> Reviewing code  structure and optimization suggestions.
		
		-> Refining technical documentation.
---
