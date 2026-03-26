#pragma once

#include "Client.hpp"
#include <string>
#include <sstream>

// --- MACROS DES CODES IRC ---
#define ERR_NOSUCHNICK      401
#define ERR_NOSUCHCHANNEL   403
#define ERR_UNKNOWNCOMMAND  421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE   433
#define ERR_NEEDMOREPARAMS  461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH  464

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
