#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>

struct Rule {
    std::string action; // "accept", "drop", "deny"
    std::string parameters;
    std::string comment;
    std::string interface; // interface name
    std::string protocol; // "tcp" or "udp"
    std::string port; // port number
};

struct Chain {
    std::string name;
    std::string type;
    std::string hook;
    std::string priority;
    std::string policy;
    std::vector<Rule> rules;
};


struct Table {
    std::string name;
    std::string type;
    std::vector<Chain> chains;
};

#endif /* STRUCTS_H */

