# Matt_daemon

A Unix daemon written in C++ that listens on port 4242 and logs all received messages. This is a 42 school project implementing proper daemon creation, signal handling, and socket communication.

## Features

- Proper Unix daemonization (double fork, setsid, chdir)
- TCP server listening on port 4242
- Up to 3 simultaneous client connections
- Signal interception and graceful shutdown
- Lock file mechanism to ensure single instance
- Logging system with multiple severity levels

## Requirements

- Linux environment (daemon operations)
- Root privileges
- C++17 compiler
- Make

## Build

```bash
make        # Build the daemon (produces MattDaemon binary)
make clean  # Remove object files
make fclean # Remove object files and binary
make re     # Clean rebuild
```

## Usage

### Using Docker (recommended)

```bash
# Start the container
docker-compose up -d --build

# Enter the container
docker exec -it matt_daemon bash

# Inside container: build and run
make && ./MattDaemon
```

### Direct execution (requires root)

```bash
sudo ./MattDaemon
```

### Connect as a client

```bash
# In another terminal
nc localhost 4242

# Type messages and press Enter
# Type "quit" to stop the daemon
```

### Check logs

```bash
tail -f /var/log/matt_daemon/matt_daemon.log
```

### Stop the daemon

```bash
# Option 1: Send quit command
echo "quit" | nc localhost 4242

# Option 2: Send signal
pkill MattDaemon

# Option 3: Manual kill
ps aux | grep MattDaemon
kill <pid>
```

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                           Client(s)                             │
│                      (nc localhost 4242)                        │
└──────────────────────────┬──────────────────────────────────────┘
                           │ TCP Socket (max 3 clients)
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                         MattDaemon                              │
│                    (Daemon Process - PID 1)                     │
├─────────────────────────────────────────────────────────────────┤
│                         MattDaemon                              │
│          (Server loop, client handling, epoll I/O)              │
├──────────────┬──────────────────────────────────────────────────┤
│ SignalHandler│              TintinReporter                      │
│  (SIGINT,    │         (Logging system with                     │
│   SIGTERM,   │          timestamps and levels)                  │
│   SIGQUIT,   │                                                  │
│   SIGHUP,    │                                                  │
│   SIGPIPE)   │                                                  │
└──────────────┴──────────────────────────────────────────────────┘
         │                        │
         ▼                        ▼
┌──────────────────┐    ┌──────────────────────────┐
│ Lock File        │    │ Log File                 │
│ /var/lock/       │    │ /var/log/matt_daemon/    │
│ matt_daemon.lock │    │ matt_daemon.log          │
└──────────────────┘    └──────────────────────────┘
```

### Components

| Component          | File                      | Responsibility                                 |
| ------------------ | ------------------------- | ---------------------------------------------- |
| **main**           | `src/main.cpp`            | Entry point, root check, daemon orchestration  |
| **MattDaemon**     | `src/matt_daemon.cpp`     | Daemon creation, server setup, client handling |
| **SignalHandler**  | `src/sighandler.cpp`      | Signal interception and graceful shutdown      |
| **TintinReporter** | `src/tintin_reporter.cpp` | Singleton logger with file output              |

---

## Core Concepts

### Daemonization Process

```
┌──────────────┐
│    Parent    │
│   Process    │
└──────┬───────┘
       │ fork()
       ▼
┌──────────────┐     exit()    ┌──────────────┐
│    Parent    │ ─────────────►│   Terminated │
└──────────────┘               └──────────────┘
       │
       ▼
┌──────────────┐
│    Child     │
│  (Session    │◄── setsid() creates new session
│   Leader)    │
└──────┬───────┘
       │ fork()
       ▼
┌──────────────┐     exit()    ┌──────────────┐
│   Session    │ ─────────────►│   Terminated │
│   Leader     │               └──────────────┘
└──────────────┘
       │
       ▼
┌──────────────┐
│   Daemon     │◄── chdir("/"), close stdin/stdout/stderr
│  (Orphan)    │
└──────────────┘
```

### Signal Handling

| Signal    | Action                             |
| --------- | ---------------------------------- |
| `SIGINT`  | Log and graceful shutdown          |
| `SIGTERM` | Log and graceful shutdown          |
| `SIGQUIT` | Log and graceful shutdown          |
| `SIGHUP`  | Log and graceful shutdown          |
| `SIGPIPE` | Log and graceful shutdown          |
| `SIGKILL` | Immediate termination (not caught) |

### Log Levels

| Level   | Usage                                   |
| ------- | --------------------------------------- |
| `LOG`   | Client messages                         |
| `DEBUG` | Debug information                       |
| `INFO`  | General information (startup, shutdown) |
| `WARN`  | Warnings (max clients reached)          |
| `ERROR` | Errors (socket failures)                |
| `FATAL` | Fatal errors (program termination)      |

### Log Format

```
[DD/MM/YYYY-HH:MM:SS] [ LEVEL ] - Message
```

Example:

```
[30/12/2025-15:44:07] [ INFO ] - Matt_daemon: Lock acquired
[30/12/2025-15:44:07] [ INFO ] - Matt_daemon: Entering daemon mode
[30/12/2025-15:44:07] [ INFO ] - Matt_daemon: Started. PID: 123
[30/12/2025-15:44:07] [ INFO ] - Matt_daemon: Server created on port 4242
[30/12/2025-15:44:10] [ INFO ] - Matt_daemon: Client connected (1/3)
[30/12/2025-15:44:15] [ LOG ] - hello world
[30/12/2025-15:44:20] [ LOG ] - quit
[30/12/2025-15:44:20] [ INFO ] - Matt_daemon: Quit command received
[30/12/2025-15:44:20] [ INFO ] - Matt_daemon: Quitting
```

---

## Data Flow / Lifecycle

### 1. Daemon Startup

```
1. Check root privileges (getuid() == 0)
2. Acquire exclusive lock on /var/lock/matt_daemon.lock
3. Daemonize:
   └─ First fork() + parent exit
   └─ setsid() - create new session
   └─ Second fork() + parent exit
   └─ chdir("/") - change to root directory
   └─ Close stdin, stdout, stderr
4. Setup signal handlers (SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGPIPE)
5. Create TCP server socket on port 4242
6. Setup epoll for I/O multiplexing
7. Enter main server loop
```

### 2. Client Connection

```
1. epoll_wait() detects incoming connection
2. accept4() new client (non-blocking)
3. Check client count < MAX_CLIENTS (3)
4. Add client fd to epoll
5. Log connection
```

### 3. Message Handling

```
1. epoll_wait() detects data on client fd
2. recv() message into buffer
3. Strip trailing \n (if present)
4. Log message with TintinReporter::log()
5. If message == "quit":
   └─ Log quit command
   └─ Set running = 0
   └─ Exit server loop
```

### 4. Graceful Shutdown

```
1. Signal received OR "quit" command
2. Exit server loop
3. MattDaemon destructor:
   └─ Close epoll fd
   └─ Close server fd
   └─ Release lock (flock LOCK_UN)
   └─ Close lock fd
   └─ Remove lock file
4. Log "Matt Daemon exiting"
```

---

## File Paths

| File                                   | Purpose                             |
| -------------------------------------- | ----------------------------------- |
| `/var/lock/matt_daemon.lock`           | Lock file (ensures single instance) |
| `/var/log/matt_daemon/matt_daemon.log` | Log file                            |

---

## Configuration

| Constant      | Value  | Description                  |
| ------------- | ------ | ---------------------------- |
| `PORT`        | `4242` | TCP server port              |
| `MAX_CLIENTS` | `3`    | Maximum simultaneous clients |

---

## Project Constraints (42 School)

- The `daemon()` function is **prohibited** - daemonization must be implemented manually
- Must run as root
- Only one instance can run at a time (enforced via lock file with `flock()`)
- "quit" command from any client terminates the daemon
- All signals must be intercepted and logged

---

## Tech Stack

- **Language**: C++17
- **Build**: Make
- **I/O Multiplexing**: epoll
- **Container**: Docker (Debian Bullseye)
