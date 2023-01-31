#pragma once

#include "Channel.hpp" // oder die Member elemente, die wir brauchen
#include <string>

#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMESLIST 366

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_TOOMANYTARGETS 407
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_UNKNOWNCOMMAND 421
#define ERR_FILEERROR 424
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERROUNEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISSMATCH 464
#define ERR_KEYSET 467
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLY 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_NOPRIVILEGES 481
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502

std::string rpl_channelmodeis( int, std::string&, std::string, std::string );
std::string rpl_notopic( int, std::string& );
std::string rpl_topic( int, std::string&, std::string& );
std::string rpl_inviting( int, std::string&, std::string, std::string& );
std::string rpl_namreply( int, std::string, std::string, Members& );

std::string err_nosuchnick( int, std::string );
std::string err_nosuchchannel( int, std::string );
std::string err_cannotsendtochan( int, std::string& );
std::string err_toomanychannels( int, std::string& );
std::string err_toomanytargets( int, std::string );
std::string err_norecipient( int, std::string& );
std::string err_notexttosend( int );
std::string err_unknowncommand( int, std::string& );
std::string err_nonicknamegiven( int );
std::string err_errouneusnickname( int, std::string& );
std::string err_nicknameinuse( int, std::string, std::string );
std::string err_usernotinchannel( int, std::string&, std::string& );
std::string err_notonchannel( int, std::string& );
std::string err_useronchannel( int, std::string&, std::string& );
std::string err_notregistered( int );
std::string err_needmoreparams( int, std::string& );
std::string err_alreadyregistred( int );
std::string err_passwdmissmatch( int , std::string);
std::string err_keyset( int, std::string& );
std::string err_channelisfull( int, std::string& );
std::string err_unknownmode( int, std::string& );
std::string err_inviteonly( int, std::string& );
std::string err_bannedfromchan( int, std::string& );
std::string err_badchannelkey( int, std::string& );
std::string err_noprivileges( int );
std::string err_chanoprivsneeded( int, std::string& );
std::string err_umodeunknownflag( int );
std::string err_usersdontmatch( int );
