#pragma once

#include "General.hpp"
#include "Channel.hpp"
#include "User.hpp"

#include <string>
#include <sstream>
#include <poll.h> // for pollfd
#include <vector>

class Server
{
  public:
    Server( int port, std::string server_password );
    ~Server( void );

     const std::string&  getPassword( void ) const;
     Users&              getUsers( void ) ;
     Channels&           getChannels( void ) ;

    void pollLoop( void );

  private:
    typedef std::vector< pollfd > PfdVector;

    PfdVector         m_pfds;
    Users             m_users;
    Channels          m_channels;
    const std::string m_server_password;

    PfdVector::iterator acceptConnections( void );
    void                flush_all_user_buffers( void );
};
