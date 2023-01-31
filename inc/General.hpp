#pragma once

// general:

#define SERVER_NAME "irc.42"
#define SERVER_TIMEOUT 3 * 60 * 1000 // minutes * seconds * microsecs per sec
#define BUFFER_SIZE 512

#include <vector>
typedef std::vector< std::string > StringVector;


// command:

#include <string>

struct Command {
    std::string     directive;
    StringVector    channels;
    StringVector    users;
    StringVector    args;
    std::string     msg;
};

StringVector    splitString( std::string, char );
Command         tokenizeCMD( std::string& );
int             isAlphaNum(  std::string );


// colors:

# define RED			"\033[31m"
# define RED_BOLD		"\033[1;31m"
# define GREEN			"\033[32m"
# define GREEN_BOLD		"\033[1;32m"
# define BLUE			"\033[34m"
# define BLUE_BOLD		"\033[1;34m"
# define PURPLE			"\033[35m"
# define PURPLE_BOLD	"\033[1;35m"
# define YELLOW			"\033[33m"
# define YELLOW_BOLD	"\033[1;33m"
# define RESET			"\033[0m"


// irc_log():

#include <iostream>

#ifndef VERBOSITY
  #define VERBOSITY 3
#endif

enum e_status {
    error,
    info,
    trace,
    dev
};

template< typename T, typename U >
void irc_log(enum e_status status, T msg, U msg2 ) {
    switch (status) {
        case error:
            if (VERBOSITY > error)
                std::cerr <<  RED_BOLD << "Error: "  << RESET << RED <<  msg << msg2 << RESET << std::endl;
            exit(-1);

        case info:
            if (VERBOSITY > info)
                std::cout << BLUE_BOLD << "Info: " << RESET << BLUE << msg << msg2 << RESET << std::endl;
            break ;

        case trace:
            if (VERBOSITY > trace)
                std::cerr << YELLOW_BOLD << "Trace: " << RESET << YELLOW << msg << msg2 << RESET << std::endl;
            break ;

        case dev:
            std::cout << GREEN_BOLD << "dev: " << GREEN << msg << msg2 << RESET << std::endl;
            break ;
    }
}
