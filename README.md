# Thread-Pooled-Server
The code is based on the multithreaded server desbribed in the text on Multithreaded Servers. The main difference is the server loop. Rather than starting a new thread per incoming connection, the connection is wrapped in a Runnable and handed off to a thread poool with a fixed number of threads.
