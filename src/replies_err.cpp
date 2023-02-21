#include "General.hpp"
#include "replies.hpp"

#include <sstream> // std::stringstream

// Used to indicate the nickname parameter supplied to a command is currently unused.
std::string err_nosuchnick( std::string nick )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NOSUCHNICK << " " << nick << " :No such nick/channel" << "\r\n";

    return ss.str();
}

// Used to indicate the given channel name is invalid.
std::string err_nosuchchannel( std::string channel_name )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NOSUCHCHANNEL << " " << channel_name << " :No such channel" << "\r\n";

    return ss.str();
}

// Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on a channel which has mode +m set and is trying to send a PRIVMSG message to that channel.
std::string err_cannotsendtochan( std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_CANNOTSENDTOCHAN << " " << channel_name << " :Cannot send to channel" << "\r\n";

    return ss.str();
}

// Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.
std::string err_toomanychannels( std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_TOOMANYCHANNELS << " " << channel_name << " :You have joined too many channels" << "\r\n";

    return ss.str();
}

// Returned to a client which is attempting to send a PRIVMSG/NOTICE using the user@host destination format and for a user@host which has several occurrences.
std::string err_toomanytargets( std::string channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_TOOMANYTARGETS << " " << channel_name << " Duplicate recipients. No message delivered" << "\r\n";

    return ss.str();
}

// Returned to a client which is attempting to send a PRIVMSG/NOTICE without specifing a destination.
std::string err_norecipient( std::string& directive )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_NORECIPIENT << " " << ":No recipient given (" << directive << ")\r\n";

    return ss.str();
}

// Returned by PRIVMSG to indicate that the message wasn't delivered for some reason.
std::string err_notexttosend( void )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_NOTEXTTOSEND << " " << ":No text to send" << "\r\n";

    return ss.str();
}

// Returned to a registered client to indicate that the command sent is unknown by the server.
std::string err_unknowncommand( std::string& directive )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_UNKNOWNCOMMAND << " " << directive << " :Unknown command" << "\r\n";

    return ss.str();
}

// BONUS
// 424     ERR_FILEERROR
//         ":File error doing <file op> on <file>"
//         - Generic error message used to report a failed file
//             operation during the processing of a message.

// Returned when a nickname parameter expected for a command and isn't found.
std::string err_nonicknamegiven( void )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NONICKNAMEGIVEN << " " << ":No nickname given" << "\r\n";

    return ss.str();
}

// Returned after receiving a NICK message which contains characters which do not fall in the defined set.
std::string err_errouneusnickname( std::string& nick )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_ERROUNEUSNICKNAME << " " << nick << " :Errouneus nickname" << "\r\n";

    return ss.str();
}

// Returned when a NICK message is processed that results in an attempt to change to a currently existing nickname.
std::string err_nicknameinuse( std::string nick, std::string old_nick )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NICKNAMEINUSE << " " << old_nick << " " << nick << " :Nickname is already in use" << "\r\n";

    return ss.str();
}

// Returned by the server to indicate that the target user of the command is not on the given channel.
std::string err_usernotinchannel( std::string& nick, std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_USERNOTINCHANNEL << " " << nick << " " << channel_name << " :They aren't on that channel" << "\r\n";

    return ss.str();
}

// Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.
std::string err_notonchannel( std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_NOTONCHANNEL << " " << channel_name << " :You're not on that channel" << "\r\n";

    return ss.str();
}

// Returned when a client tries to invite a user to a channel they are already on.
std::string err_useronchannel( std::string& nick, std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_USERONCHANNEL << " " << nick << " " << channel_name << " :is already on channel" << "\r\n";

    return ss.str();
}

// Returned by the server to indicate that the client must be registered before the server will allow it to be parsed in detail.
std::string err_notregistered( void )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_NOTREGISTERED << " " << ":You have not registered" << "\r\n";

    return ss.str();
}

// Returned by the server by numerous commands to indicate to the client that it didn't supply enough parameters.
std::string err_needmoreparams( std::string& directive )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NEEDMOREPARAMS << " " << directive << " :Not enough parameters" << "\r\n";

    return ss.str();
}

// Returned by the server to any link which tries to change part of the registered details (such as password or user details from second USER message).
std::string err_alreadyregistred( void )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_ALREADYREGISTRED << " " << ":You may not reregister" << "\r\n";

    return ss.str();
}

// Returned to indicate a failed attempt at registering a connection for which a password was required and was either not given or incorrect.
std::string err_passwdmissmatch( std::string pwd )
{
    std::stringstream ss;

    // ss  << ":" << SERVER_NAME << " " << ERR_PASSWDMISSMATCH << " " << pwd << " :Password incorrect" << "\r\n";
    ss << ERR_PASSWDMISSMATCH << " " << pwd << " " << "PASS" << " :" << "Password incorrect\r\n";

    return ss.str();
}

// Returned to indicate that the channel key has already been set prior.
std::string err_keyset( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_KEYSET << " " << channel << " :Channel key already set" << "\r\n";

    return ss.str();
}

// Returned to indicate that the channel has reached its maximum capacity of members.
std::string err_channelisfull( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_CHANNELISFULL << " " << channel << " :Cannot join channel (+l)" << "\r\n";

    return ss.str();
}

// Returned to indicate that the mode flag is undefined.
std::string err_unknownmode( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_UNKNOWNMODE << " " << channel << " :is unknown mode char to me" << "\r\n";

    return ss.str();
}

// Returned to indicate that the channel is invite-only configured.
std::string err_inviteonly( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_INVITEONLY << " " << channel << " :Cannot join channel (+i)" << "\r\n";

    return ss.str();
}

// Returned to indicate that the client has been banned from the channel.
std::string err_bannedfromchan( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_BANNEDFROMCHAN << " " << channel << " :Cannot join channel (+b)" << "\r\n";

    return ss.str();
}

// Returned to indicate that the provided channel key is incorrect.
std::string err_badchannelkey( std::string &channel )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_BADCHANNELKEY << " " << channel << " :Cannot join channel (+k)" << "\r\n";

    return ss.str();
}

// Any command requiring operator privileges to operate must return this error to indicate the attempt was unsuccessful.
std::string err_noprivileges( void )
{
    std::stringstream ss;

    ss  << ":" << SERVER_NAME << " " << ERR_NOPRIVILEGES << " " << ":Permission Denied- You're not an IRC operator" << "\r\n";

    return ss.str();
}

// Any command requiring 'chanop' privileges (such as MODE messages) must return this error if the client making the attempt is not a chanop on the specified channel.
std::string err_chanoprivsneeded( std::string& channel_name )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_CHANOPRIVSNEEDED << " " << channel_name << " :You're not channel operator" << "\r\n";

    return ss.str();
}

// Returned by the server to indicate that a MODE message was sent with a nickname parameter and that the a mode flag sent was not recognized.
std::string err_umodeunknownflag( void )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_UMODEUNKNOWNFLAG << " " << ":Unknown MODE flag" << "\r\n";

    return ss.str();
}

// Error sent to any user trying to view or change the user mode for a user other than themselves.
std::string err_usersdontmatch( void )
{
    std::stringstream ss;

    ss << ":" << SERVER_NAME << " " << ERR_USERSDONTMATCH << " " << ":Cant change mode for other users" << "\r\n";

    return ss.str();
}
