#ifndef UTILITES_H
#define UTILITES_H

#include <vector>
#include <iostream>

class Utilites
{
public:
    static std::string ExecToStr(const char* cmd);
    static bool FileExists(const std::string& filename);
    static bool SrvNftablesIsLife(void);
    static bool SetSrvNftablesState(bool Enabled);
    //Получить список файлов по паттерну "/var/lib/nftables/*.nft"
    static std::vector<std::string> GetFilesByPattern(const std::string& pattern);
    static void test();
};

#endif /* UTILITES_H */

