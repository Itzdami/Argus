#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    close(sock);

    return (result == 0);
}

int main() {
    const char* target_ip = "127.0.0.1";
    int start_port = 1;
    int end_port = 1024;
    
    std::cout << "Starting Argus scan on " << target_ip << "...\n\n";

    for (int port = start_port; port <= end_port; ++port) {
        if (scan_port(target_ip, port)) {
            std::cout << "[+] Port " << port << " is OPEN\n";
        }
    }

    std::cout << "\nScan Complete.\n";
    return 0;
}