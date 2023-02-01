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

     std::string&  getPassword( void ) ;
     Users&        getUsers( void ) ;
     Channels&     getChannels( void ) ;

    void          erase_user_by_nick(  std::string& nickname );

    void pollLoop( void );

  private:
    typedef std::vector< pollfd > PfdVector;

    PfdVector   m_pfds;
    Users       m_users;
    Channels    m_channels;
    std::string m_server_password;

    PfdVector::iterator acceptConnections( void );
    // int                 parseCMD( Users::iterator, std::string );
    // int                 handleCMD( Users::iterator, Command& );
};
