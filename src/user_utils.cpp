#include "User.hpp"

#include <sstream> // std::stringstream

std::string build_full_client_identifier( User& user )
{
    std::stringstream ss;

    ss  << user.get_nick() << "!" << user.get_name() << "@" << user.get_user_ip();

    return ss.str();
}

std::string build_welcome( std::string nick )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " 001 " << nick
        << " :Welcome to our ft_irc " << nick << "!" << "\r\n";

    return ss.str();
}

std::string build_join_channel( User& new_member, std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << build_full_client_identifier(new_member) << " JOIN " << channel_name << "\r\n";

    return ss.str();
}

std::string build_part_channel( User& user, std::string& channel_name, std::string& msg )
{
    std::stringstream ss;

    ss << ":" << build_full_client_identifier(user) << " PART " << channel_name << " " << msg << "\r\n";

    return ss.str();
}

std::string build_new_nickname( User& user, std::string& nick )
{
    std::stringstream ss;

    ss << ":" << build_full_client_identifier(user) << " NICK :" << nick << "\r\n";

    return ss.str();
}

std::string build_privmsg( User& user, Command& cmd, std::string &other_user )
{
    std::stringstream ss;

    ss << ":" << build_full_client_identifier(user) << " " << cmd.directive << " " << other_user << " " << cmd.msg << "\r\n";

    return ss.str();
}

std::string build_pingpong( void )
{
    std::stringstream ss;

    ss << "PONG " << SERVER_NAME << "\r\n";

    return ss.str();
}

Users::iterator find_user_by_nick( std::string nick, Users& users )
{
    Users::iterator it = users.begin();
    for (; it != users.end(); it++)
    {
        if (nick == it->second.get_nick())
            break ;
    }
    return it;
}

std::string check_duplicates( StringVector& vec )
{
    for (StringVector::iterator first_it = vec.begin(); first_it != vec.end(); first_it++)
    {
        for (StringVector::iterator second_it = first_it + 1; second_it != vec.end(); second_it++)
        {
            if (*first_it == *second_it)
                return *first_it;
        }
    }
    return "";
}
