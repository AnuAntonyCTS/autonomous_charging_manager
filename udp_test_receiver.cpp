#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    const int port = 30000;
    const int buffer_size = 65536;

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0)
    {
        std::cerr << "Failed to create UDP socket\n";
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(
            socket_fd,
            reinterpret_cast<sockaddr*>(&server_address),
            sizeof(server_address)) < 0)
    {
        std::cerr << "Failed to bind UDP port "
                  << port << "\n";

        close(socket_fd);
        return 1;
    }

    std::cout
        << "UDP test receiver listening on port "
        << port << "\n";

    uint8_t buffer[buffer_size];

    while (true)
    {
        sockaddr_in client_address{};
        socklen_t client_length = sizeof(client_address);

        ssize_t received =
            recvfrom(
                socket_fd,
                buffer,
                sizeof(buffer),
                0,
                reinterpret_cast<sockaddr*>(&client_address),
                &client_length);

        if (received < 0)
        {
            std::cerr << "Failed to receive UDP packet\n";
            continue;
        }

        std::cout
            << "\nReceived UDP packet: "
            << received
            << " bytes\n";

        std::cout << "HEX:\n";

        for (ssize_t i = 0; i < received; ++i)
        {
            printf("%02X ", buffer[i]);

            if ((i + 1) % 16 == 0)
            {
                printf("\n");
            }
        }

        printf("\n");

        std::cout << "ASCII:\n";

        for (ssize_t i = 0; i < received; ++i)
        {
            if (buffer[i] >= 32 && buffer[i] <= 126)
            {
                std::cout << static_cast<char>(buffer[i]);
            }
            else
            {
                std::cout << '.';
            }
        }

        std::cout << "\n";
    }

    close(socket_fd);

    return 0;
}
