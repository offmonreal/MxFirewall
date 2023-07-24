#include "Utilites.h"
#include <unistd.h>

#include <dirent.h>
#include <fnmatch.h>
#include <string>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "NftablesParser.h"

bool Utilites::FileExists(const std::string& filename)
{
    return (access(filename.c_str(), F_OK) != -1);
}

std::vector<std::string> getFilesByPatternRecursive(const std::string& directory, const std::string& filePattern)
{
    std::vector<std::string> files;
    DIR* dir = opendir(directory.c_str());

    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            // Если найден файл или директория
            if (entry->d_type == DT_REG) {
                if (!fnmatch(filePattern.c_str(), entry->d_name, 0)) {
                    files.push_back(directory + "/" + entry->d_name);
                }
            }
            else if (entry->d_type == DT_DIR) {
                // Исключаем директории "." и ".."
                if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
                    std::string newDirectory = directory + "/" + entry->d_name;
                    std::vector<std::string> newFiles = getFilesByPatternRecursive(newDirectory, filePattern);
                    files.insert(files.end(), newFiles.begin(), newFiles.end());
                }
            }
        }
        closedir(dir);
    }
    // Исключение не бросается, вместо этого функция просто пропускает недоступные директории
    // else {
    //    throw std::runtime_error("Could not open directory");
    //}

    return files;
}

std::vector<std::string> Utilites::GetFilesByPattern(const std::string& pattern)
{

    std::size_t lastSlashPos = pattern.rfind('/');
    if (lastSlashPos == std::string::npos) {
        throw std::invalid_argument("Invalid pattern");
    }

    std::string directory = pattern.substr(0, lastSlashPos);
    std::string filePattern = pattern.substr(lastSlashPos + 1);

    return getFilesByPatternRecursive(directory, filePattern);
}

int manage_service(const std::string& service_name, bool start)
{
    std::string operation = start ? "start" : "stop";

    // Проверка доступности systemctl
    if (access("/bin/systemctl", F_OK) != -1) {
        std::string command = "pkexec systemctl " + operation + " " + service_name;
        return system(command.c_str());
    }
        // Проверка доступности rc-service
    else if (access("/sbin/rc-service", F_OK) != -1) {
        std::string command = "pkexec rc-service " + service_name + " " + operation;
        return system(command.c_str());
    }
    else {
        return -999; // Иначе, возвращаем false, так как неизвестно, как управлять службой
    }
}

int is_service_active(const std::string& service_name)
{
    // Проверка доступности systemctl
    if (access("/bin/systemctl", F_OK) != -1) {
        std::string command = "pkexec systemctl is-active --quiet " + service_name;
        return system(command.c_str());
    }
        // Проверка доступности rc-service
    else if (access("/sbin/rc-service", F_OK) != -1) {
        std::string command = "pkexec rc-service " + service_name + " status";
        return system(command.c_str());
    }

    // Иначе, возвращаем false, так как неизвестно, как проверить статус службы
    return -999;

}

bool Utilites::SrvNftablesIsLife(void)
{
    //int xxresult = system("rc-service nftables status");
    //std::cout << "aaaaaa" << xxresult << "\n";

    int result = is_service_active("nftables"); //system("systemctl is-active --quiet nftables");

    if (result == 0) {
        std::cout << "Service nftables running\n";
        return true;
    }
    else {
        std::cout << "Service nftables not running, result: " << result << "\n";
        return false;
    }
    return false;
}

bool Utilites::SetSrvNftablesState(bool Enabled)
{
    int result = 0;
    result = manage_service("nftables", Enabled);

    /*
    if (Enabled)
        result = system("systemctl start nftables");
    else
        result = system("systemctl stop nftables");
     */
    if (result != 0)
        return false;

    return true;
}

std::string Utilites::ExecToStr(const char* cmd)
{
    //Результат выполения команты в string
    
    std::array<char, 4096> buffer;
    
    std::string result;
    
    std::unique_ptr<FILE, decltype(&pclose) > pipe(popen(cmd, "r"), pclose);
    
    if (!pipe) 
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
    {
        result += buffer.data();
    }
    
    return result;
}

void Utilites::test()
{
    NftablesParser parser;
    
    std::string script = ExecToStr("pkexec nft list ruleset");
    parser.ParseScriptFromString(script);
    /*
    //auto table = parser.parseScript("/home/maxim/Desktop/nft.conf");
    if (FileExists("/etc/nftables.conf"))
        parser.parseScript("/home/maxim/Desktop/nft.conf");
    else if (FileExists("/etc/nftables.nft"))
        parser.parseScript("/etc/nftables.nft");

    //printParsedScript(table);
    * */

}
