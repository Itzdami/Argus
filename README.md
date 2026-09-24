# Argus — a multithreaded TCP port scanner 
written in C++ using raw POSIX sockets,
capable of concurrently scanning 1000+ ports
with per-connection timeouts and thread-safe output.

## How it works

For each port in the given range, Argus:
1. Opens a TCP socket and attempts a full `connect()` handshake against the target
2. If the connection succeeds, sends a minimal HTTP `GET /` request and listens
   briefly for a response (banner grab)
3. Reports the port as open, along with any banner received

Each port is scanned on its own thread, with results printed under a shared mutex to avoid garbled output when multiple threads finish at the same time.

## Usage

Build:

g++ main.cpp -o argus -pthread

Run:

./argus <target_ip> <start_port> <end_port>

Example:

./argus 127.0.0.1 1 1024

DISCLAIMER!!

Only scan hosts you own or have explicit permission to test.
Use against `127.0.0.1` your own local network,
or designated test targets like `scanme.nmap.org`
