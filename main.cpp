#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>                                        
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>

std::mutex print_mutex;

bool scan_port(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;
    
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    sockaddr_in target{};
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip, &target.sin_addr);
    
    int result = connect(sock, (struct sockaddr*)&target, sizeof(target));
    
    if (result == 0) {
        const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
        send(sock, request, strlen(request), 0);

        char buffer[256] = {0};

        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1,0);
        if (bytes_received > 0) {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "    Banner: " << buffer << "\n";
        }
    }

    close(sock);

    return (result == 0);
}

void scan_port_threaded(const char* ip, int port) {
    if (scan_port(ip, port)) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "[+] Port " << port << " is OPEN\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <target_ip> <start_port> <end_port>\n";
        std::cerr << "Example: " << argv[0] << " 127.0.0.1 1 1024\n";
        return 1;
    }

    const char* target_ip = argv[1];
    int start_port = std::atoi(argv[2]);
    int end_port = std::atoi(argv[3]);

    std::cout << "Starting Argus scan on " << target_ip << "...\n\n";
    std::vector<std::thread> threads;
    for (int port = start_port; port <= end_port; ++port) {
        threads.emplace_back(scan_port_threaded, target_ip, port);
    }
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nScan Complete.\n";
    return 0;
}