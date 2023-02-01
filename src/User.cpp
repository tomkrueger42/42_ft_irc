#include "User.hpp"

#include "Server.hpp"
#include "Channel.hpp"
#include "replies.hpp"

#include <sys/socket.h> // accept()
#include <arpa/inet.h> // net_ntoa()

std::string&    User::get_name()    { return m_username; }
std::string&    User::get_user_ip() { return m_ipAddress; }
std::string&    User::get_nick()    { return m_nickname; }
int             User::get_fd()      { return m_fd; }
bool            User::get_is_registered() { return m_isRegistered; }

User::User( int fd, sockaddr_in addr )
    : m_username("*"), m_nickname("*"), m_isAuth(false), m_isRegistered(false),
   m_fd(fd), m_addr(addr)
{
    // irc_log(trace, "User::User(): ", fd);

    m_ipAddress = inet_ntoa(m_addr.sin_addr);
}

User::~User( void ) {}

void User::parseCMD( std::string buffer, Server& server )
{
    irc_log(trace, "User::parseCMD(): ", m_nickname);

    buffer_in += buffer;
    while (buffer_in.find("\r\n") != buffer_in.npos)
    {
        std::string tmp = buffer_in.substr(buffer_in.find("\r\n") + 2, buffer_in.npos);
        buffer_in = buffer_in.substr(0, buffer_in.find("\r\n"));

        // Command cmd = tokenizeCMD(buffer_in);

        handleCMD(tokenizeCMD(buffer_in), server);
        buffer_in = tmp;
    }
}

void User::handleCMD( Command cmd, Server& serv )
{
    irc_log(trace, "User::handleCMD() ", cmd.directive);

    if (cmd.directive == "PASS")
        auth_user(cmd, serv); // needs to erase user if password is wrong
    else if (m_isAuth == false)
    {
        err_passwdmissmatch(m_fd, m_nickname); // not sure if user needs to be deleted if PASS is not sent immediately
        // serv.erase_user_by_nick(m_nickname);
        return ;
    }
    else
    {
        if (cmd.directive == "NICK")
            set_nickname(cmd, serv.getUsers(), serv.getChannels());
        else if (cmd.directive == "USER")
            set_username(cmd, cmd.users);
        else if (cmd.directive == "CAP")
            set_user_metadata(cmd.users);
        else if (m_isRegistered == false)
            err_notregistered(m_fd);
        else
        {
            if (cmd.directive == "PING")
                pingpong();
            else if ((cmd.directive == "PRIVMSG" || cmd.directive == "NOTICE") && cmd.channels.empty())
                send_privmsg(cmd, serv.getUsers());
            else if (cmd.directive == "PRIVMSG" || cmd.directive == "NOTICE")
                send_channel_msg(cmd, serv.getChannels());
            else if (cmd.directive == "JOIN")
                join_channel(cmd, serv.getChannels());
            else if (cmd.directive == "PART")
                part_channel(cmd, serv.getChannels());
            else if (cmd.directive == "KICK")
                kick_user(cmd, serv.getUsers(), serv.getChannels());
            else if (cmd.directive == "MODE")
                mode(cmd, serv.getChannels());
            else if (cmd.directive == "TOPIC")
                topic(cmd, serv.getChannels());
            else if (cmd.directive == "INVITE")
                invite(cmd, serv.getChannels(), serv.getUsers());
            else if (cmd.directive == "QUIT")
                quit(cmd.msg, serv);
            else if (cmd.directive == "SQUIT")
                throw CloseServer();
            else
                err_unknowncommand(m_fd, cmd.directive);
        }
    }
}

void User::auth_user( Command& cmd,  Server& serv )
{
    irc_log(trace, "User::auth_user(): ", m_nickname);

    if (m_isAuth == false)
    {
        if (cmd.users.size() == 0)
            err_needmoreparams(m_fd, cmd.directive);
        else if (cmd.users.back() == serv.getPassword())
            m_isAuth = true;
        else
            err_passwdmissmatch(m_fd, m_nickname);
    }
    else
    {
        err_alreadyregistred(m_fd);
    }
    if (m_isAuth == false)
        serv.getUsers().erase(find_user_by_nick(m_nickname, serv.getUsers())); // könnte das einen SEGFAULT geben?
}

void User::set_nickname( Command& cmd, Users& users, Channels& channels )
{
    irc_log(trace, "User::set_nickname(): ", cmd.users.front());

    if (cmd.users.empty())
    {
        err_needmoreparams(m_fd, cmd.directive);
    }
    else if (!isAlphaNum(cmd.users.front()))
    {
        err_errouneusnickname(m_fd, cmd.users.front());
        return ;
    }
    Users::iterator u_it = find_user_by_nick(cmd.users.front(), users);
    if (u_it != users.end())
    {
        err_nicknameinuse(m_fd, cmd.users.front(), m_nickname);
        if (m_nickname != "*")
        {
            return ;
        }
        cmd.users.front() += "1"; // adds "1" when new user logs in and the nick is already in use
    }
    build_new_nickname(m_fd, *this, cmd.users.front());
    for (Channels::iterator ch_it = channels.begin(); ch_it != channels.end(); ch_it++)
    {
        ch_it->change_member_nickname(m_nickname, cmd.users.front());
    }
    m_nickname = cmd.users.front();
    register_user();
    // return 0;
}

void User::set_username(  Command& cmd,  StringVector& strVec )
{
    irc_log(trace, "User::set_username()", "");

    if (strVec.empty())
    {
        err_needmoreparams(m_fd, cmd.directive);
        return ;
    }
    if (isAlphaNum(strVec.front()))
    {
        m_username = strVec.front();
    }
    register_user();
}

void User::register_user( void )
{
    if (m_isRegistered == false && m_nickname != "*" && m_username != "*")
    {
        build_welcome(m_fd, m_nickname);
        m_isRegistered = true;
    }
}

void User::set_user_metadata(  StringVector& strVec )
{
    m_metadata.insert(m_metadata.end(), strVec.begin(), strVec.end());
}

void User::send_privmsg( Command& cmd, Users& users )
{
    irc_log(trace, "User::send_privmsg(): ", cmd.msg);

    if (cmd.users.empty())
    {
        err_norecipient(m_fd, cmd.directive);
        return ;
    }
    else if (cmd.msg.empty())
    {
        err_notexttosend(m_fd);
        return ;
    }
    else if (!check_duplicates(cmd.users).empty())
    {
        err_toomanytargets(m_fd, check_duplicates(cmd.users));
        return ;
    }
    for (StringVector::iterator name_it = cmd.users.begin(); name_it != cmd.users.end(); name_it++)
    {
        Users::iterator recipient = find_user_by_nick(*name_it, users);
        if (recipient != users.end())
        {
            build_privmsg(recipient->m_fd, *this, cmd, recipient->get_nick());
        }
        else
        {
            err_nosuchnick(m_fd, *name_it);
        }
    }
}

void User::send_channel_msg( Command& cmd, Channels& channels )
{
    irc_log(trace, "User::send_channel_msg(): ", cmd.msg);

    for (StringVector::iterator ch_name_it = cmd.channels.begin(); ch_name_it != cmd.channels.end(); ch_name_it++)
    {
        Channels::iterator ch_it = find_channel_by_name(*ch_name_it, channels);
        if (ch_it == channels.end())
        {
            err_nosuchchannel(m_fd, *ch_name_it);
            continue ;
        }
        std::string msg = build_privmsg(0, *this, cmd, ch_it->get_name());
        int err = ch_it->channel_message(m_nickname, msg);
        if (err == ERR_NOTONCHANNEL)
            err_notonchannel(m_fd, ch_it->get_name());
        else if (err == ERR_CANNOTSENDTOCHAN)
            err_cannotsendtochan(m_fd, ch_it->get_name());
    }
}

void User::join_channel( Command& cmd, Channels& channels )
{
    irc_log(trace, "User::join_channel(): ", m_nickname);

    if (cmd.channels.empty())
    {
        err_needmoreparams(m_fd, cmd.directive);
        return ;
    }

    for (StringVector::iterator ch_name_it = cmd.channels.begin(); ch_name_it != cmd.channels.end(); ch_name_it++)
    {
        Channels::iterator ch_it = find_channel_by_name(*ch_name_it, channels);
        if (ch_it == channels.end())
        {
            if (is_valid_channel_name(*ch_name_it))
            {
                err_nosuchchannel(m_fd, *ch_name_it);
                continue ;
            }
            channels.push_back(*ch_name_it);
            ch_it = std::prev(channels.end());
        }
        std::string channel_key = "";
        if (!cmd.users.empty())
            channel_key = cmd.users.front();
        switch (ch_it->add_member(*this, false, channel_key))
        {
        case ERR_CHANNELISFULL:
            err_channelisfull(m_fd, ch_it->get_name());
            break ;
        case ERR_INVITEONLY:
            err_inviteonly(m_fd, ch_it->get_name());
            break ;
        case ERR_BADCHANNELKEY:
            err_badchannelkey(m_fd, ch_it->get_name());
            break ;
        }
    }
}

void User::part_channel( Command& cmd, Channels& channels )
{
    irc_log(trace, "User::part_channel(): ", m_nickname);

    for (StringVector::iterator ch_name_it = cmd.channels.begin(); ch_name_it != cmd.channels.end(); ch_name_it++) 
    {
        Channels::iterator ch_it = find_channel_by_name(*ch_name_it, channels);
        if (ch_it == channels.end())
        {
            err_nosuchchannel(m_fd, *ch_name_it);
            continue ;
        }
        switch (ch_it->remove_member(m_nickname, cmd.msg))
        {
        case ERR_NOTONCHANNEL:
            err_notonchannel(m_fd, ch_it->get_name());
            break ;
        case -1:
            channels.erase(ch_it); // channel is empty, will be deleted
            break ;
        }
    }
}

void User::part_all_channels( std::string msg, Channels& channels )
{
    irc_log(trace, "User::part_all_channels(): ", m_nickname);

    for (Channels::iterator ch_it = channels.begin(); ch_it != channels.end(); ch_it++)
    {
        ch_it->remove_member(m_nickname, msg);
    }
}

void User::kick_user( Command& cmd, Users& users, Channels& channels )
{
    irc_log(trace, "User::kick_user(): ", m_nickname);

    if (cmd.channels.empty() || cmd.users.empty())
    {
        err_needmoreparams(m_fd, cmd.directive);
        return ;
    }

    for (StringVector::iterator u_nick_it = cmd.users.begin(); u_nick_it != cmd.users.end(); u_nick_it++) // parsing through all usernames we received
    {
        Users::iterator kick_it = find_user_by_nick(*u_nick_it, users);
        if (kick_it == users.end())
        {
            err_nosuchnick(m_fd, *u_nick_it); // the user does not exist on the server
            continue ;
        }

        StringVector::iterator ch_name_it = cmd.channels.begin();
        for ( ; ch_name_it != cmd.channels.end(); ch_name_it++) // parsing through all channel names we received
        {

            Channels::iterator ch_it = find_channel_by_name(*ch_name_it, channels);
            if (ch_it == channels.end())
            {
                err_nosuchchannel(m_fd, *ch_name_it);
                continue ;
            }
            switch (ch_it->kick_member(m_nickname, kick_it->get_nick(), cmd.msg))
            {
            case ERR_NOTONCHANNEL:
                err_notonchannel(m_fd, ch_it->get_name());
                break ;
            case ERR_CHANOPRIVSNEEDED:
                err_chanoprivsneeded(m_fd, ch_it->get_name());
                break ;
            case ERR_USERNOTINCHANNEL:
                err_usernotinchannel(m_fd, kick_it->get_nick(), ch_it->get_name());
                break ;
            case -1:
                channels.erase(ch_it); // channel is empty, will be deleted
            }
        }
    }
}

void User::mode( Command& cmd, Channels& channels )
{
    irc_log(trace, "User::mode(): ", m_nickname);

    if (cmd.channels.empty() || cmd.args.empty())
    {
        return ;
        err_needmoreparams(m_fd, cmd.directive); // WeeChat sometimes sends mode requests without arguments for no appearant reason, with other clients this error message should be sent before returning out of the function, obviously
    }
    Channels::iterator ch_it = find_channel_by_name(cmd.channels.front(), channels);
    if (ch_it == channels.end())
    {
        err_nosuchchannel(m_fd, cmd.channels.front());
        return ;
    }

    int err = ch_it->channel_modes(m_nickname, cmd.args, cmd.users);
    if (err == ERR_UNKNOWNMODE)
        err = ch_it->member_modes(m_nickname, cmd.users, cmd.args); // no valid channel mode found for given mode flag, checking member modes
    switch (err)
    {
    case ERR_NOTONCHANNEL:
        err_notonchannel(m_fd, ch_it->get_name());
        break ;
    case ERR_USERNOTINCHANNEL:
        err_usernotinchannel(m_fd, cmd.users.front(), ch_it->get_name());
        break ;
    case ERR_NEEDMOREPARAMS:
        err_needmoreparams(m_fd, cmd.directive);
        break ;
    case ERR_KEYSET:
        err_keyset(m_fd, ch_it->get_name()); // does not quite work, yet!!
        break ;
    case ERR_UNKNOWNMODE:
        err_unknownmode(m_fd, ch_it->get_name());
        break ;
    case ERR_CHANOPRIVSNEEDED:
        err_chanoprivsneeded(m_fd, ch_it->get_name());
        break ;
    }
}

void User::topic( Command& cmd, Channels& channels )
{
    irc_log(trace, "User::topic(): ", m_nickname);

    Channels::iterator ch_it = find_channel_by_name(cmd.channels.front(), channels);
    if (ch_it == channels.end())
    {
        err_nosuchchannel(m_fd, cmd.channels.front());
        return ;
    }

    if (cmd.msg.empty())
    {
        switch (ch_it->topic(m_nickname))
        case ERR_NOTONCHANNEL:
            err_notonchannel(m_fd, ch_it->get_name());
    }
    else
    {
        switch (ch_it->set_topic(m_nickname, cmd.msg))
        {
        case ERR_NOTONCHANNEL:
            err_notonchannel(m_fd, ch_it->get_name());
        case ERR_CHANOPRIVSNEEDED:
            err_chanoprivsneeded(m_fd, ch_it->get_name());
        }
    }
}

void User::invite(Command& cmd, Channels& channels, Users& users)
{
    irc_log(trace, "User::invite(): ", m_nickname);

    if (cmd.channels.empty() || cmd.users.empty())
    {
        return ;
        err_needmoreparams(m_fd, cmd.directive); // WeeChat always sends two invite requests, the second one without arguments for no appearant reason. For release code, the error message should be sent before returning out of the function, obviously
    }

    Users::iterator u_it = find_user_by_nick(cmd.users.front(), users);
    if (u_it == users.end())
    {
        err_nosuchnick(m_fd, cmd.users.front());
        return ;
    }

    Channels::iterator ch_it = find_channel_by_name(cmd.channels.front(), channels);
    if (ch_it == channels.end())
    {
        err_nosuchchannel(m_fd, cmd.channels.front());
        return ;
    }

    switch (ch_it->invite_user(*u_it, m_nickname))
    {
    case ERR_NOTONCHANNEL:
        err_notonchannel(m_fd, ch_it->get_name());
        break ; 
            break ; 
        break ; 
    case ERR_USERONCHANNEL:
        err_useronchannel(m_fd, u_it->get_nick(), ch_it->get_name());
        break ; 
            break ; 
        break ; 
    case ERR_CHANNELISFULL:
        err_channelisfull(m_fd, ch_it->get_name());
        break ;
    case ERR_CHANOPRIVSNEEDED:
        err_chanoprivsneeded(m_fd, ch_it->get_name());
        break ;
    }
}

void User::quit( std::string msg,  Server& serv )
{
    irc_log(trace, "User::quit(): ", m_nickname);

    part_all_channels(msg, serv.getChannels());
    // serv.erase_user_by_nick(m_nickname);
    throw CloseUser();
}

void User::pingpong( void )
{
    build_pingpong(m_fd);
}
