#pragma once

#include "Channel.hpp"
#include "General.hpp"

#include <string>
#include <sstream>
#include <poll.h> // pollfd
#include <netinet/in.h> // struct sockaddr_in, socklen_t
#include <exception>

class Server;

#include <map>
class User;
typedef std::map< int, User > Users;

class User
{
  public:
    User( void );
    User( int, sockaddr_in );
    ~User( void );
    void    parseCMD( std::string, Server& );
    void    handleCMD( Command, Server& );
    void    set_user_metadata(  StringVector& strVec );
    void    auth_user( Command& cmd,  Server& serv );
    void    set_nickname( Command&, Users&, Channels& );
    void    set_username(  Command& cmd,  StringVector& strVec );
    void    send_privmsg( Command&, Users& );
    void    send_channel_msg( Command&, Channels& );
    void    join_channel( Command&, Channels& );
    void    part_channel( Command&, Channels& );
    void    part_all_channels( std::string, Channels& );
    void    kick_user( Command&, Users&, Channels& );
    void    mode( Command&, Channels& );
    void    topic( Command&, Channels& );
    void    invite( Command&, Channels&, Users&);
    void    quit( std::string msg,  Server& serv );
    void    pingpong( void );


    void flush_buffer( void );

    std::string&  get_user_ip();
    std::string&  get_nick();
    std::string&  get_name();
    bool          get_is_registered();
    int           get_fd();
    short int&    get_revents();
    pollfd        get_socket();
    std::string   buffer_in;
    std::string   buffer_out;


    class CloseServer : public std::exception
    {
      public:
        const char* what() const throw()
        {
            return "Closing server by ";
        }
    };

    class CloseUser : public std::exception
    {
      public:
        const char* what() const throw()
        {
            return "Closing user with nickname ";
        }
    };

  private:

    void register_user( void );

    std::string m_username;
    std::string m_nickname;

    bool        m_isAuth;
    bool        m_isRegistered;
    StringVector  m_metadata;

    int         m_fd;
    std::string m_ipAddress;
    sockaddr_in m_addr;
    // socklen_t   m_len;
};

// user_utils.cpp
std::string     build_full_client_identifier( User& );
std::string     build_welcome( std::string );
std::string     build_join_channel( User&, std::string& );
std::string     build_part_channel( User&, std::string&, std::string& );
std::string     build_new_nickname( User&, std::string& );
std::string     build_privmsg( User&, Command&, std::string& );
std::string     build_pingpong( );
Users::iterator find_user_by_nick( std::string, Users& );
std::string     check_duplicates( StringVector& );
