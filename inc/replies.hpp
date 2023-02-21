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

std::string rpl_channelmodeis( std::string&, std::string, std::string );
std::string rpl_notopic( std::string& );
std::string rpl_topic( std::string&, std::string& );
std::string rpl_inviting( std::string&, std::string, std::string& );
std::string rpl_namreply( std::string, std::string, Members& );

std::string err_nosuchnick( std::string );
std::string err_nosuchchannel( std::string );
std::string err_cannotsendtochan( std::string& );
std::string err_toomanychannels( std::string& );
std::string err_toomanytargets( std::string );
std::string err_norecipient( std::string& );
std::string err_notexttosend( );
std::string err_unknowncommand( std::string& );
std::string err_nonicknamegiven( );
std::string err_errouneusnickname( std::string& );
std::string err_nicknameinuse( std::string, std::string );
std::string err_usernotinchannel( std::string&, std::string& );
std::string err_notonchannel( std::string& );
std::string err_useronchannel( std::string&, std::string& );
std::string err_notregistered( );
std::string err_needmoreparams( std::string& );
std::string err_alreadyregistred( );
std::string err_passwdmissmatch( std::string);
std::string err_keyset( std::string& );
std::string err_channelisfull( std::string& );
std::string err_unknownmode( std::string& );
std::string err_inviteonly( std::string& );
std::string err_bannedfromchan( std::string& );
std::string err_badchannelkey( std::string& );
std::string err_noprivileges( );
std::string err_chanoprivsneeded( std::string& );
std::string err_umodeunknownflag( );
std::string err_usersdontmatch( );
