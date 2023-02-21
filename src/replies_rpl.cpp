#include "General.hpp"
#include "replies.hpp"

#include <sstream> // std::stringstream

// 301     RPL_AWAY
//                 "<nick> :<away message>"

// 305     RPL_UNAWAY
//                 ":You are no longer marked as being away"
// 306     RPL_NOWAWAY
//                 ":You have been marked as being away"

//         - These replies are used with the AWAY command (if
//             allowed).  RPL_AWAY is sent to any client sending a
//             PRIVMSG to a client which is away.  RPL_AWAY is only
//             sent by the server to which the client is connected.
//             Replies RPL_UNAWAY and RPL_NOWAWAY are sent when the
//             client removes and sets an AWAY message.

// Returned to notify about changed channel modes.
std::string rpl_channelmodeis( std::string& channel_name, std::string mode, std::string params )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << RPL_CHANNELMODEIS << " RPL_CHANNELMODEIS " << channel_name << " " << mode << " " << params << "\r\n";

    return ss.str();
}

// Returned to indicate that the channel has no configured topic.
std::string rpl_notopic( std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << RPL_NOTOPIC << " RPL_NOTOPIC " << channel_name << " :No topic is set" << "\r\n";

    return ss.str();
}

// When sending a TOPIC message to determine the channel topic, one of two replies is sent. If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.
std::string rpl_topic( std::string& channel_name, std::string& topic )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << RPL_TOPIC << " RPL_TOPIC " << channel_name << " :" << topic << "\r\n";

    return ss.str();
}


// Returned by the server to indicate that the attempted INVITE message was successful and is being passed onto the end client.
std::string rpl_inviting( std::string& channel_name, std::string caller, std::string& nick )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << RPL_INVITING << " " << channel_name << " " << nick  << " :" << caller << "\r\n";

    return ss.str();
}

#include "Channel.hpp"
#include "User.hpp"

// To reply to a NAMES message, a reply pair consisting of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the server back to the client.  If there is no channel found as in the query, then only RPL_ENDOFNAMES is returned.  The exception to this is when a NAMES message is sent with no parameters and all visible channels and contents are sent back in a series of RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark the end.
std::string rpl_namreply( std::string nick, std::string channel_name, Members& members )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << RPL_NAMREPLY << " " << nick
        << " = " << channel_name << " :" << get_names(members) << "\r\n";

    ss  << ":" << SERVER_NAME << " " << RPL_ENDOFNAMESLIST << " " << nick
        << " " << channel_name << " :End of NAMES list." << "\r\n";

    return ss.str();
}
