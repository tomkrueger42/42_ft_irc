#include "Channel.hpp"

Channels::iterator find_channel_by_name( std::string& channel_name, Channels& channels )
{
    Channels::iterator it = channels.begin();
    for (; it != channels.end(); it++)
    {
        if (channel_name == it->get_name())
            break ;
    }
    return it;
}

int is_valid_channel_name(  std::string& ch_name )
{
    if (ch_name.length() >= 2 && ch_name[0] == '#' && isAlphaNum(ch_name.substr(1)))
        return 0;
    return 1;
}

#include "User.hpp"
typedef std::pair< User, struct MemberPrivileges > Member;
typedef std::vector< Member >           Members;

std::string get_names( Members& members )
{
    std::string names;
    for (Members::iterator it = members.begin(); it < members.end(); it++)
    {
        if (it->second.chanop == 1)
            names += "@";
        names += it->first.get_nick() + " ";
    }
    return names;
}
