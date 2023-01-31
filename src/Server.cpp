#include "Server.hpp"

#include "replies.hpp"

#include <netinet/in.h> // for struct sockaddr_in
#include <sys/socket.h> // for socket(), bind(), setsockopt(), listen()
#include <fcntl.h> // for fcntl()
#include <unistd.h> // for close()

Server::Server( int port,  std::string server_password )
    : m_server_password(server_password)
{
    irc_log(trace, "Server ructor(): ", port);

    m_socket = {0, POLLIN, 0};

    if ((m_socket.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        irc_log(error, "m_socket() failed!", "");

    int on = 1;
    if (setsockopt(m_socket.fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
        irc_log(error, "setsockopt() failed!", "");

    if (fcntl(m_socket.fd, F_SETFL, O_NONBLOCK) < 0)
        irc_log(error, "fcntl() failed!", "");

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(m_socket.fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        irc_log(error, "bind() failed!", "");

    if (listen(m_socket.fd, SOMAXCONN) == -1)
        irc_log(error, "listen() failed!", "");

    // User server(m_socket.fd, m_users.size());
    // m_users.push_back(server);
}

Server::~Server()
{
    irc_log(trace, "Server destructor()", "");

    close(m_socket.fd);
    for (Users::iterator it = m_users.begin(); it < m_users.end(); it++)
        close(it->get_fd()); // closing open sockets
}

std::string& Server::getPassword( void ) 
{
    return m_server_password;
}

Users& Server::getUsers( void ) 
{
    return m_users;
}

Channels& Server::getChannels( void ) 
{
    return m_channels;
}

void Server::acceptConnections( void )
{
    irc_log(trace, "Server::acceptConnections()", "");

    while (true)
    {
        sockaddr_in user_addr;
        socklen_t   user_addr_len = sizeof(user_addr);

        int user_fd = accept(m_socket.fd, (sockaddr*)&user_addr, &user_addr_len);
        if (user_fd < 0)
            return ;
        pollfd      user_socket = {user_fd, POLLIN, -1};

        User new_user(user_socket, user_addr, user_addr_len);
        irc_log(info, "new user fd: ", new_user.get_fd());
        m_users.push_back(user_socket, user_addr, user_addr_len);
    }
}

void Server::erase_user_by_nick(  std::string& nickname )
{
    Users::iterator user_it = find_user_by_nick(nickname, m_users);
    close(user_it->get_fd()); // broken pipe??
    m_users.erase(user_it);
}

void Server::pollLoop()
{
    while (true)
    {
        irc_log(info, "_______________POLLING_______________\n", "");
        std::vector<pollfd> pfds(1, m_socket);
        pfds.reserve(m_users.size());
        for (Users::iterator it = m_users.begin(); it < m_users.end(); it++)
            pfds.push_back(it->get_socket());
        // pollfd fds[m_users.size()];
        // for (size_t i = 0; i < m_users.size(); i++)
            // fds[i] = m_users[i].get_socket();
        switch (poll(pfds.data(), pfds.size(), SERVER_TIMEOUT))
        {
        case -1:
            irc_log(error, "Server::pollLoop(): ", "poll error");
            break ;
        case 0:
            irc_log(error, "Server::pollLoop(): ", "poll timeout");
            break ;
        default:
            acceptConnections();
            for (Users::iterator user_it = m_users.begin(); user_it < m_users.end(); user_it++)
            {
                ++user_it->get_revents();
                if (user_it == m_users.begin()       // skipping server
                    || user_it->get_revents() == 0) // skipping recently accepted users before polling them
                    continue ;

                char buffer[BUFFER_SIZE];
                std::memset(&buffer, 0, sizeof(buffer));
                if (recv(user_it->get_fd(), &buffer, sizeof(buffer), 0) == 0) // rc == 0 -> connection closed by User
                {
                    user_it->quit(" :Connection lost", *this);
                }
                else if (user_it->parseCMD(buffer, *this))
                    return ;
            }
        }
    }
}
