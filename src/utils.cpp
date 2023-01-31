#include "General.hpp"

#include <string> // std::string

StringVector splitString( std::string buffer, char delimiter )
{
    StringVector channel_names;
    while (buffer.find(delimiter) != buffer.npos)
    {
        channel_names.push_back(buffer.substr(0, buffer.find(delimiter)));
        buffer.erase(0, buffer.find(delimiter) + 1);
    }
    channel_names.push_back(buffer.substr(0, buffer.npos));
    buffer.erase(0, buffer.npos);
    return (channel_names);
}

Command tokenizeCMD( std::string& buffer )
{
    Command cmd;
    StringVector str_vec = splitString(buffer, ' ');

    if (buffer.find(':') != buffer.npos)
        cmd.msg = buffer.substr(buffer.find(':'));

    for (StringVector::iterator it = str_vec.begin(); it != str_vec.end(); it++)
    {
        if (it == str_vec.begin())                                              // command
            cmd.directive = *it;
        else if (it->find('#') == 0 && 0 != it->npos)                           // channel(s)
            cmd.channels = splitString(*it, ',');
        else if (it->find(':') == 0 && 0 != it->npos)                           // message is already parsed
            break ;
        else if ((it->find('-') == 0 || it->find('+') == 0) && 0 != it->npos)   // argument(s)
            cmd.args.push_back(*it);
        else if (cmd.users.empty())                                             // nickname(s)
            cmd.users = splitString(*it, ',');
    }
    return cmd;
}

int isAlphaNum(  std::string str )
{
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (std::isalnum(str[i]) == 0)
            return 0;
    }
    return 1;
}


