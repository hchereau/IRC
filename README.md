_This project has been created as part of the 42 curriculum by yookyeoc, hucherea and keait-he._

---

## 📖 Description

This project consists of creating our own IRC server. The server is developed strictly adhering to the C++ 98 standard. It is capable of handling multiple clients concurrently without blocking, utilizing non-blocking I/O operations and a single `poll()` (or equivalent) call to manage all socket operations.

It implements the core features of an IRC server, allowing users to authenticate, set a nickname and a username, join channels, and send or receive private messages. It also includes channel operator privileges with the commands KICK, INVITE, TOPIC, and MODE.

---

## 📚 Good to Know

* **Message Structure:** Format `<prefix> <command> <params> \r\n` (max 512 bytes).
* Consists of up to three main parts: the prefix (OPTIONAL), the command, and the parameters (maximum of 15).
* The presence of a prefix is indicated by a `:` as the first character of the message.
* There is no space between the colon and the prefix.
* The prefix is used by servers to indicate the origin of the message.
* The only valid prefix for clients is their registered nickname.

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

---

## 📚 Resources and References

listing classic references related to the topic (documentation, articles, tutorials etc.), as well as a description of how AI was used specifying for which tasks and which parts of the project.

<keait-he's ressources:
	- https://www.rfc-editor.org/rfc/rfc2812#section-2.3 To know about Messages' structures
	- https://cplusplus.com/reference For functions' usage
	- 



---

## 🤖 AI Usage Disclosure

hucherea:

	- Used AI as a complement to official documentation (RFC 2812) to clarify protocol specifics.

	- Applied AI for code review after initial development to ensure "Clean Code" standards and improve logic efficiency.

	- Researched optimal C++ 98 patterns for network programming.

---
