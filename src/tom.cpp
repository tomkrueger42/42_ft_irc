#include "Server.hpp"

int main( int argc, char **argv )
{
    if (argc != 3)
        irc_log(error, "Required arguments for server startup: ", "./ircserv <port> <password>");
    long int port = std::strtol(argv[1], NULL, 10);
    if (port < 0 || port > 65535)
        irc_log(error, "Invalid port: ", "Port must be in range [0, 65536)");
    Server serv(port, argv[2]);
    serv.pollLoop();
    return 0;
}
