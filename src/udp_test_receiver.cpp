#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    constexpr int PORT = 30000;
    constexpr int BUFFER_SIZE = 65535;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(
            sock,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0)
    {
        perror("bind");
        close(sock);
        return 1;
    }

    std::cout << "UDP test receiver listening on port "
              << PORT << std::endl;

    uint8_t buffer[BUFFER_SIZE];

    while (true)
    {
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);

        ssize_t received = recvfrom(
            sock,
            buffer,
            BUFFER_SIZE,
            0,
            reinterpret_cast<sockaddr*>(&sender),
            &sender_len);

        if (received < 0)
        {
            perror("recvfrom");
            break;
        }

        std::cout << "\nReceived "
                  << received
                  << " bytes\n";

        std::cout << "HEX:\n";

        for (ssize_t i = 0; i < received; ++i)
        {
            std::cout
                << std::hex
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(buffer[i])
                << " ";

            if ((i + 1) % 16 == 0)
                std::cout << "\n";
        }

        std::cout << std::dec << "\n\n";

        std::cout << "ASCII/PAYLOAD:\n";

        for (ssize_t i = 0; i < received; ++i)
        {
            if (buffer[i] >= 32 && buffer[i] <= 126)
                std::cout << static_cast<char>(buffer[i]);
            else
                std::cout << '.';
        }

        std::cout << "\n";
    }

    close(sock);

    return 0;
}
