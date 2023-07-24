#ifndef NFTABLESPARSER_H
#define NFTABLESPARSER_H

#include <fstream>
#include <string>
#include <vector>
#include <set>


#include "Structs.h"

class NftablesParser 
{
public:
    NftablesParser() = default;
    std::vector<Table>  parseScript(const std::string &filePath);
    std::vector<Table>  ParseScriptFromString(const std::string &script);

    void printTable(const Table &table);

private:
    std::vector<std::string> tokenize(const std::string &line, char delimiter) const;
    std::string processText(const std::string& text);
    std::vector<std::string> processSpecialCharacters(std::vector<std::string>& lines);
    Chain parseChain(const std::vector<std::string> &tokens);
    void parseChain(Chain &chain, const std::vector<std::string> &tokens);
    Rule parseRule(const std::vector<std::string> &tokens);
    Table parseTable(const std::vector<std::string>& tokens);
    bool isRule(const std::string& token) const;
    std::string trim(const std::string& str);
private:
    //Для контроля вставки уникальных путей файлов
    std::vector<std::string> uniqueStrings;
    std::set<std::string> seenStrings;
    void insertUnique(const std::string& str);

};


#endif // NFTABLESPARSER_H
