# Magnanimity
An unopinionated orchestration library that supports RPC-like functionality with minimal overhead and no code-generation.

Limitations:
- no hand-holding
- restricted to interoperability with localhost only 

At the mile-high-level, this is a cross-language multi-platform semaphore library.
1) Requests are serialized to disk using message-pack, protobuf, json, or any mechanism you choose.
2) Replies are serialized to disk using message-pack, protobuf, json, or any mechanism you choose.
3) The watchdog only performs initialization and routine cleanup (timestamp based; more rules forthcoming)
4) It is up to each client to serialize the request to disk in a uniform location
5) It is up to the the server to read that request from disk
6) It is up to the the server to also write the response
7) Both client and server are responsible for writing heartbeats and timestamps into Magnanimous shared memory for syncronization

Request lifetimes, response lifetimes, and watch-dog services, are all handled by the Magnanimity server.
Each client reads and writes into shared memory in accordance with precise rules:
- no external library to link with (so any language that can read/write shared-memory is compatible)
- completely agnostic on how to serialize data
- if all clients and servers obey the data access rules, then there is no possibility of memory race conditions.
- See the color-coded PDF/spreadsheet for access rules:</br>
https://github.com/kwonus/Magnanimity/blob/master/Magnanimity.pdf
	- Blue is writable by servers
	- Green is writable by clients/listeners
	- All other slots are read-only

- low overhead

- super low-latency

Admittedly, the memory accessibility rules are tedious, but documented (and pretty straight-forward).
However, once that is well-understood, there is almost zero complexity and minimal bias (after all, it is just a sophisticated semaphore-server with a watch-dog)
Easy to lean (while this server is C++; your favorite language is all you'll need to know! YAML-driven configuration coming soon)

<br>One Dependency:
- XVMem: (https://github.com/kwonus/XVMem)

  