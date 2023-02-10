#include "Channel.hpp"

#include "General.hpp"
#include "User.hpp"
#include "replies.hpp"

// public:

Channel::Channel( std::string& name ) : m_name(name), m_flags(0), m_member_limit(LONG_MAX) {}

// Adds new_member to m_members
int Channel::add_member( User& new_member, bool is_invitation, std::string key = "" )
{
    // irc_log(trace, "Channel::add_member(): ", new_member.get_nick());

    if (find_member(new_member.get_nick()) != m_members.end())
        return ERR_USERONCHANNEL; // useronchannel
    else if (m_flags & MBR_LIMIT_CHANNEL_FLAG && m_members.size() >= m_member_limit)
        return ERR_CHANNELISFULL; // channelisfull
    else if ((m_flags & PRIVATE_CHANNEL_FLAG
                || m_flags & INV_ONLY_CHANNEL_FLAG) && is_invitation == false)
        return ERR_INVITEONLY; // private channel or inviteonly
    else if (m_flags & SET_KEY_CHANNEL_FLAG && is_invitation == false)
    {
        if (key.empty() || key != m_channel_key)
            return ERR_BADCHANNELKEY; // badchannelkey
    }

    broadcast(build_join_channel(new_member.get_fd(), new_member, m_name), m_members.end());

    m_members.push_back(std::make_pair(new_member, MemberPrivileges()));
    if (m_members.size() == 1)
        m_members.back().second.chanop = 1;

    topic(m_members.back().first.get_nick());

    rpl_namreply(new_member.get_fd(), new_member.get_nick(), m_name, m_members);
    return (0);
}

int Channel::invite_user(User& user, std::string& caller)
{
    // irc_log(trace, "Channel::invite_user(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (!is_chanop(m_it))
        return ERR_CHANOPRIVSNEEDED; // chanoprivsneeded

    if (m_flags & MBR_LIMIT_CHANNEL_FLAG && m_members.size() >= m_member_limit)
        return ERR_CHANNELISFULL; // channelisfull

    int err = add_member(user, true);
    if (err != 0)
        return ERR_USERONCHANNEL; // useronchannel
    
    rpl_inviting(m_it->first.get_fd(), m_it->first.get_nick(), m_name, user.get_nick());
    return 0;
}

void Channel::change_member_nickname( std::string& caller, std::string& new_nick )
{
    // irc_log(trace, "Channel::change_member_nickname(): ", new_nick);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ;

    broadcast(build_new_nickname(0, m_it->first, new_nick), m_members.end());
    m_it->first.get_nick() = new_nick;
}

int Channel::remove_member( std::string& caller, std::string& msg )
{
    // irc_log(trace, "Channel::remove_member(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    broadcast(build_part_channel(m_it->first.get_fd(), m_it->first, m_name, msg), m_it);

    m_members.erase(m_it);
    if (m_members.size() == 0)
        return -1; // channel will be destroyed
    if (num_of_chanops() == 0)
    {
        m_members.front().second.chanop = 1; // new chanop needs to know that he became chanop
        broadcast(rpl_channelmodeis(0, m_name, "+o", m_members.front().first.get_nick()), m_members.end());
    }
    return 0;
}

int Channel::kick_member( std::string& caller, std::string& nick, std::string& message )
{
    // irc_log(trace, "Channel::kick_member(): ", nick);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (is_chanop(m_it) == 0)
        return ERR_CHANOPRIVSNEEDED; // chanoprivsneeded

    int err = remove_member(nick, message);
    if (err == ERR_NOTONCHANNEL)
        return ERR_USERNOTINCHANNEL; // usernotinchannel (needs to be converted from ERR_NOTONCHANNEL to ERR_USERNOTINCHANNEL cause we change the perspective from one member to another)
    else if (err == -1)
        return -1; // channel empty, will be destroyed

    return 0;
}

int Channel::channel_message( std::string& caller, std::string& msg )
{
    // irc_log(trace, "Channel::channel_message(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if ((m_flags & MODERATED_CHANNEL_FLAG && !m_it->second.speaker) && !is_chanop(m_it))
        return ERR_CANNOTSENDTOCHAN; // cannotsendtochan

    broadcast(msg, m_it);
    return 0;
}

int Channel::topic( std::string& caller )
{
    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (m_topic.empty())
        rpl_notopic(m_it->first.get_fd(), m_name);
    else
        rpl_topic(m_it->first.get_fd(), m_name, m_topic);

    return 0;
}

int Channel::set_topic( std::string& caller, std::string& new_topic )
{
    // irc_log(trace, "Channel::set_topic(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (m_flags & TPC_CHANOP_CHANNEL_FLAG && !is_chanop(m_it))
        return ERR_CHANOPRIVSNEEDED; // chanoprivsneeded

    m_topic = new_topic.substr(1);
    return 0;
}

// Set modes concerning the members of a channel
int Channel::member_modes( std::string& caller, StringVector& target_nick, StringVector& args )
{
    // irc_log(trace, "Channel::member_modes(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (is_chanop(m_it) == 0)
        return ERR_CHANOPRIVSNEEDED; // chanoprivsneeded

    if (target_nick.empty()) // problem: if there is an unknown flag char, we will only send err_unknownmode when target_nick is not empty
        return ERR_NEEDMOREPARAMS;

    Members::iterator target_it = find_member(target_nick.front());
    if (target_it == m_members.end())
        return ERR_USERNOTINCHANNEL;

    for (StringVector::iterator arg_it = args.begin(); arg_it != args.end(); arg_it++)
    {
        if (arg_it->length() != 2)
            return ERR_UNKNOWNMODE; // command too short
        if (arg_it->at(1) == 'o')
            target_it->second.chanop = (arg_it->at(0) == '+');
        else if (arg_it->at(1) == 'v')
            target_it->second.speaker = (arg_it->at(0) == '+');
        else
            return ERR_UNKNOWNMODE; // command not found

        broadcast(rpl_channelmodeis(m_it->first.get_fd(), m_name, *arg_it, target_it->first.get_nick()), m_it);
    }
    return 0;
}

// Set modes concernig a channel
int Channel::channel_modes( std::string& caller, StringVector& args, StringVector& params ) 
{
    // irc_log(dev, "Channel::channel_modes(): ", caller);

    Members::iterator m_it = find_member(caller);
    if (m_it == m_members.end())
        return ERR_NOTONCHANNEL; // notonchannel

    if (is_chanop(m_it) == 0)
        return ERR_CHANOPRIVSNEEDED; // chanoprivsneeded

    for (StringVector::iterator arg_it = args.begin(); arg_it != args.end(); arg_it++)
    {
        if (arg_it->length() != 2)
            return ERR_UNKNOWNMODE; // command too short
        if (arg_it->at(1) == 'i')                           // invite only channel
            change_modes(*arg_it, INV_ONLY_CHANNEL_FLAG);
        else if (arg_it->at(1) == 't')                      // only channel operators can change the channels topic
            change_modes(*arg_it, TPC_CHANOP_CHANNEL_FLAG);
        else if (arg_it->at(1) == 'm')                      // channel is moderated, only speakers and chanops can broadcast
            change_modes(*arg_it, MODERATED_CHANNEL_FLAG);
        else if (arg_it->at(1) == 'l')                      // channel member limit
        {
            change_modes(*arg_it, MBR_LIMIT_CHANNEL_FLAG);
            if (!params.empty())
            {
                m_member_limit = std::strtol(params.front().c_str(), NULL, 10);
                broadcast(rpl_channelmodeis(m_it->first.get_fd(), m_name, *arg_it, params.front()), m_it);
                break ;
            }
        }
        else if (arg_it->at(1) == 'k')                      // channel key
        {
            if (params.empty() && m_channel_key.empty())
                return ERR_NEEDMOREPARAMS; // needmoreparams
            else if (!params.empty())
                m_channel_key = params.front();
            else if (!params.empty() && !m_channel_key.empty())
                return ERR_KEYSET; // keyset
            change_modes(*arg_it, SET_KEY_CHANNEL_FLAG);
            broadcast(rpl_channelmodeis(m_it->first.get_fd(), m_name, *arg_it, m_channel_key), m_it);
            break ;
        }
        else
            return ERR_UNKNOWNMODE; // command not found

        broadcast(rpl_channelmodeis(m_it->first.get_fd(), m_name, *arg_it, ""), m_it);
    }
    return 0;
}

std::string& Channel::get_name( void )
{
    return m_name;
}

// private:

// Returns iterator to channel member with nickname nick or end() iterator
Members::iterator Channel::find_member( std::string& nick )
{
    for (Members::iterator m_it = m_members.begin(); m_it != m_members.end(); m_it++)
        if (nick == m_it->first.get_nick())
            return (m_it);
    return (m_members.end());
}

// Broadcasts message msg to all channel m_members except for the sender if sender is not the end iterator
void Channel::broadcast(  std::string msg, Members::iterator sender )
{
    for (Members::iterator m_it = m_members.begin(); m_it != m_members.end(); m_it++)
        if (m_it != sender)
            send(m_it->first.get_fd(), msg.c_str(), msg.length(), 0);
}

// Accesses the m_members chanop variable
int Channel::is_chanop( Members::iterator m_it )
{
    return m_it->second.chanop;
}

// Counts number of channel operators
size_t Channel::num_of_chanops( void )
{
    size_t chanops = 0;
    for (Members::iterator m_it = m_members.begin(); m_it != m_members.end(); m_it++)
        if (m_it->second.chanop)
            ++chanops;
    return chanops;
}

// Manipulates the channel flags
void Channel::change_modes( std::string& m, unsigned short int flag )
{
    // irc_log(trace,"Channel::change_modes(): ", m);

    if (m.at(0) == '+')
    {
        if (!(m_flags & flag))
            m_flags += flag;
    }
    else if (m.at(0) == '-')
    {
        if (m_flags & flag)
            m_flags -= flag;
    }
}
