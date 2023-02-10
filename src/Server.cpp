#include "Server.hpp"

#include "replies.hpp"

#include <netinet/in.h> // for struct sockaddr_in
#include <sys/socket.h> // for socket(), bind(), setsockopt(), listen()
#include <fcntl.h> // for fcntl()
#include <unistd.h> // for close()

Server::Server( int port,  std::string server_password )
    : m_server_password(server_password)
{
    irc_log(trace, "Server constructor(): ", port);

    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        irc_log(error, "socket() failed!", "");

    int on = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
        irc_log(error, "setsockopt() failed!", "");

    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
        irc_log(error, "fcntl() failed!", "");

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        irc_log(error, "bind() failed!", "");

    if (listen(server_fd, SOMAXCONN) == -1)
        irc_log(error, "listen() failed!", "");

    pollfd server_socket = {server_fd, POLLIN, 0};
    m_pfds.clear();
    m_pfds.push_back(server_socket);
}

Server::~Server()
{
    irc_log(trace, "Server destructor()", "");

    for (PfdVector::iterator it = m_pfds.begin(); it < m_pfds.end(); it++)
        close(it->fd); // closing open sockets
    m_pfds.clear();
    m_users.clear();
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

std::vector< pollfd >::iterator Server::acceptConnections( void )
{
    irc_log(trace, "Server::acceptConnections()", "");

    sockaddr_in user_addr;
    socklen_t   user_addr_len = sizeof(user_addr);

    int user_fd = accept(m_pfds.front().fd, (sockaddr*)&user_addr, &user_addr_len);
    if (user_fd >= 0)
    {
        pollfd      user_socket = {user_fd, POLLIN, -1};
        m_pfds.push_back(user_socket);
        m_users.push_back(User(user_fd, user_addr));

        irc_log(info, "new user fd: ", user_fd);
    }
    return m_pfds.begin();
}

Users::iterator find_user_by_fd( int fd, Users& users )
{
    Users::iterator it = users.begin();
    for (; it != users.end(); it++)
    {
        if (fd == it->get_fd())
            break ;
    }
    return it;
}

#include "User.hpp"
void Server::pollLoop()
{
    while (true)
    {
        irc_log(info, "_______________POLLING_______________\n", "");

        switch (poll(m_pfds.data(), m_pfds.size(), SERVER_TIMEOUT))
        {
        case -1:
            irc_log(error, "Server::pollLoop(): ", "poll error");
            break ;
        case 0:
            irc_log(error, "Server::pollLoop(): ", "poll timeout");
            break ;
        default:
            irc_log(dev, "m_pfds size: ", m_pfds.size());
            for (PfdVector::iterator pfd_it = m_pfds.begin(); pfd_it < m_pfds.end(); pfd_it++)
            {
                if (pfd_it->revents == 0)
                    continue ;

                if (pfd_it == m_pfds.begin())
                {
                    pfd_it = acceptConnections(); // push_back invalidates iterator
                    continue ;
                }

                Users::iterator u_it = find_user_by_fd(pfd_it->fd, m_users);

                try
                {
                    char buffer[BUFFER_SIZE];
                    std::memset(&buffer, 0, sizeof(buffer));
                    if (!recv(pfd_it->fd, &buffer, sizeof(buffer), 0))
                        u_it->quit(" :Connection lost", *this);
                    else
                        u_it->parseCMD(buffer, *this);
                }
                catch( const User::CloseUser& e )
                {
                    close(pfd_it->fd);
                    m_pfds.erase(pfd_it);
                    m_users.erase(u_it);
                    irc_log(info, e.what(), u_it->get_nick());
                }
                catch ( const User::CloseServer& e )
                {
                    irc_log(info, e.what(), u_it->get_nick());
                    return ;
                }
            }
        }
    }
}
