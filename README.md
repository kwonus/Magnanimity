# Magnanimity
An unopinionated orchestration library that supports RPC-like functionality with minimal overhead and no code-generation.

Limitations:
- no hand-holding
- restricted to interoperability with localhost only 

At the mile-high-level, this is a cross-language multi-platform semaphore library.
1) Requests are serialized to disk using message-pack, protobuf, json, or any mechanism you choose.
2) Replies are serialized to disk using message-pack, protobuf, json, or any mechanism you choose.

Syncronization of these events, response lifetimes, watch-dog services, releasing request slots, are all handled by the Magnanimity server.
Each client reads and writes into shared memory in accordance with precise rules:
- no external library to link with (so works with any language that can read/write shared-memory)

- completely agnostic on how to serialize data

- if all clients and servers obey the data access rules, there is no possibility of memory race conditions.

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

  