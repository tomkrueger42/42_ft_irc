#pragma once

#include "General.hpp"

#include <string>

#define PRIVATE_CHANNEL_FLAG    1
#define INV_ONLY_CHANNEL_FLAG   2
#define TPC_CHANOP_CHANNEL_FLAG 4
#define MODERATED_CHANNEL_FLAG  8
#define MBR_LIMIT_CHANNEL_FLAG 16
#define SET_KEY_CHANNEL_FLAG   32

struct MemberPrivileges {
    short int chanop;
    short int speaker;
} ;

#include <vector>
class User;
typedef std::pair< User*, struct MemberPrivileges > Member;
typedef std::vector< Member >           Members;

class Channel
{
  public:
    Channel( std::string& );
    int                 add_member( User*, bool, std::string );
    int                 invite_user(User*, std::string&);
    void                change_member_nickname( std::string&, std::string& );
    int                 remove_member( std::string&, std::string& );
    int                 kick_member( std::string&, std::string&, std::string& );
    int                 channel_message( std::string&, std::string& );
    int                 topic( std::string& );
    int                 set_topic(std::string& nick, std::string& );
    int                 member_modes( std::string&, StringVector&, StringVector& );
    int                 channel_modes( std::string&, StringVector&, StringVector& );

    // Accessors
    std::string&        get_name( void );

  private:
    Members::iterator   find_member( std::string& );
    void                broadcast(  std::string );
    void                broadcast(  std::string, Members::iterator );
    int                 is_chanop( Members::iterator );
    size_t              num_of_chanops( void );
    void                change_modes( std::string&, unsigned short int );

    std::string         m_name;
    std::string         m_topic;
    Members             m_members;
    unsigned short int  m_flags;
    unsigned long       m_member_limit;
    std::string         m_channel_key;
};

#include <vector>
typedef std::vector< Channel > Channels;

// channel_utils.cpp
Channels::iterator  find_channel_by_name( std::string&, Channels& );
int                 is_valid_channel_name(  std::string& );
std::string         get_names( Members& );
