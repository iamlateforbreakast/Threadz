# Threadz

A small collection of C experiments exploring Linux systems programming: non-blocking I/O with `epoll`, and multithreading with `pthreads` and `timerfd`. Each file is a standalone program used to try out a specific technique.

## Contents

### `serverz.c`
A minimal edge-triggered `epoll` HTTP server (based on the classic `man 7 epoll` example). It:
- Binds and listens on a port passed as a command-line argument
- Accepts connections and adds them to an `epoll` instance in non-blocking, edge-triggered mode
- Replies to any incoming data with a fixed `200 OK` HTML response

```sh
./serverz <port>
# e.g.
./serverz 8080
```

### `socketz.c`
A simple non-blocking TCP client that connects to a server, waits on `epoll` for the connection and incoming data, and prints whatever it receives.

By default it targets `127.0.0.1:22` — update the `SERVER`/`PORT` defines to point at `serverz` (or another service) before running.

### `threadz.c`
A `pthread` demo with two worker threads:
- **Thread 1** uses `timerfd` + `select` to wake up on a fixed period, logging wall-clock and CPU time each cycle
- **Thread 2** prints a message and exits immediately

Illustrates basic thread creation, detaching, and timing utilities (`gettimeofday`, `clock`).

## Building

```sh
make
```

See the `Makefile` for the exact targets and flags.

## Requirements
- Linux (uses `epoll`, `timerfd`, and other Linux-specific APIs — won't build on macOS/BSD without changes)
- A C compiler (gcc/clang) and `make`
- pthreads (`-lpthread`)

## Status
These are learning/reference programs rather than a packaged library or application — expect rough edges (see notes below) rather than production-hardened code.

### Known issues
- `serverz.c`: writes `sizeof(reply)` instead of `strlen(reply)`, sending a trailing null byte; the reply can also be sent more than once per connection if data arrives in multiple reads.
- `socketz.c`: single `recv()` call assumes all data arrives in one read, which isn't guaranteed over TCP.
- `threadz.c`: calls both `pthread_detach` and `pthread_join` on the same threads, which is undefined behavior — pick one.

