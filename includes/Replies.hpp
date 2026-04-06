#pragma once

#include "Client.hpp"
#include <string>
#include <sstream>

// --- MACROS DES CODES IRC ---
#define ERR_NOSUCHNICK      401
#define ERR_NOSUCHCHANNEL   403
#define ERR_NOTONCHANNEL    442
#define ERR_UNKNOWNCOMMAND  421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE   433
#define ERR_NEEDMOREPARAMS  461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH  464
#define ERR_UNKNOWNCOMMAND  421
#define ERR_PASSWDMISMATCH  464
#define ERR_NORECIPIENT     411
#define ERR_NOTEXTTOSEND    412
#define ERR_CHANNELISFULL   471
#define ERR_BADCHANNELKEY   475
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL     442
#define ERR_CHANOPRIVSNEEDED 482

#define RPL_TOPIC           332
#define RPL_NAMREPLY        353
#define RPL_ENDOFNAMES      366

#define RPL_WELCOME         "001"

// --- CLASSE UTILITAIRE ---
class Reply {
    public:
        // Envoie une erreur formatée
        static void error(Client* client, int code, const std::string& command, const std::string& msg);
        
        // Envoie le message de bienvenue officiel
        static void welcome(Client* client);

        // Optionnel: Une fonction pour envoyer un message brut si besoin
        static void raw(Client* client, const std::string& rawMsg);
};
