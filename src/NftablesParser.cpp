#include "NftablesParser.h"

#include <algorithm>
#include <sstream>
#include <iterator>
#include <numeric>
#include <iostream>
#include <unordered_set>
#include "Utilites.h"

void NftablesParser::insertUnique(const std::string& str)
{
    if (seenStrings.insert(str).second) {
        uniqueStrings.push_back(str);
    }
}

std::vector<Table> NftablesParser::ParseScriptFromString(const std::string &script)
{
    std::vector<Table> Tables;
    
    std::cout << script;
    
        std::string text;
        text = processText(script);
        std::cout << text;

    std::istringstream stream(text);

    std::string line;
    std::vector<std::string> tokens;
    Table *currentTable = nullptr;
    Chain* currentChain = nullptr;
    Rule* currentRule = nullptr;

    while (std::getline(stream, line)) {
        tokens = tokenize(line, ' ');

        if (!tokens.empty()) {
            if (tokens[0] == "table") {
                Tables.push_back(parseTable(tokens)); // Create and add a new table
                currentTable = &Tables.back(); // Update current table pointer
            }
            else if (tokens[0] == "chain" && currentTable != nullptr) {
                currentTable->chains.push_back(parseChain(tokens)); // Create and add a new chain
                currentChain = &currentTable->chains.back(); // Update current chain pointer
            }
            else if ((tokens[0] == "type" || tokens[0] == "policy") && currentChain != nullptr) {
                //Параметры дополнительные Chain
                parseChain(*currentChain, tokens);
            }
            else if (tokens[0] == "comment" && currentRule != nullptr) {
                //Это комментрай к предыдущему правилу
            }
            else if (isRule(tokens[0]) && currentChain != nullptr) {
                //Признаки что это НОВОЕ правило в текущем Chain

                currentChain->rules.push_back(parseRule(tokens));
                currentRule = &currentChain->rules.back();
            }
            else {
                //То что мы несмогли распознать и соответсвенно обработать
            }
        }

    }
    for(auto x : Tables)
    {
                printTable(x);

    }

    return Tables;
}

std::vector<Table> NftablesParser::parseScript(const std::string& filePath)
{
    std::vector<Table> Tables;
    uniqueStrings.clear();
    seenStrings.clear();
    insertUnique(filePath);

    for (auto Path : uniqueStrings) {
        std::ifstream file(Path);
        if (!file.is_open())
            throw std::runtime_error("Failed to open the script file");

        // Read the entire file into a string
        std::string text((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

        // Process the entire text
        text = processText(text);
        std::cout << text;

        // Create a new string stream from the processed text
        std::istringstream stream(text);

        std::string line;
        std::vector<std::string> tokens;
        Table table;
        Chain* currentChain = nullptr;
        Rule* currentRule = nullptr;

        while (std::getline(stream, line)) {
            tokens = tokenize(line, ' ');


            if (!tokens.empty()) {
                if (tokens[0] == "table") {
                    table = parseTable(tokens);
                    Tables.push_back(table);
                }
                else if (tokens[0] == "chain") {
                    table.chains.push_back(parseChain(tokens)); // Create and add a new chain
                    currentChain = &table.chains.back();
                }
                else if ((tokens[0] == "type" || tokens[0] == "policy") && currentChain != nullptr) {
                    //Параметры дополнительные Chain
                    parseChain(*currentChain, tokens);

                }
                else if (tokens[0] == "comment" && currentRule != nullptr) {
                    //Это комментрай к предыдущему правилу
                }
                else if (isRule(tokens[0])) {
                    //Признаки что это НОВОЕ правило в текущем Chain

                    currentChain->rules.push_back(parseRule(tokens));
                    currentRule = &currentChain->rules.back();
                }
                else if (tokens[0] == "include" && tokens.size() == 2) {

                    // Удаляем кавычки
                    tokens[1].erase(std::remove(tokens[1].begin(), tokens[1].end(), '\"'), tokens[1].end());

                    std::vector<std::string> files = Utilites::GetFilesByPattern(tokens[1]);

                    for (auto path : files)
                        insertUnique(path);

                    std::cout << "FINDE FILES " << files.size() << "\n";
                }
                else {
                    //То что мы несмогли распознать и соответсвенно обработать
                }
            }

        }

        printTable(table);


    }
    return Tables;

}

Table NftablesParser::parseTable(const std::vector<std::string>& tokens)
{
    Table table;

    if (tokens.size() >= 3 && tokens[0] == "table") {
        table.name = tokens[1];
        table.type = tokens[2];
    }
    return table;
}

std::string NftablesParser::processText(const std::string& text)
{
    std::string processedText = text;

    // заменить все табы на пробелы
    std::replace(processedText.begin(), processedText.end(), '\t', ' ');

    // Разбиваем текст на строки
    std::vector<std::string> lines = tokenize(processedText, '\n');


    // Удаляем комментарии и пробелы в начале и конце каждой строки, а также пустые строки или строки, состоящие только из пробелов
    for (auto it = lines.begin(); it != lines.end();) {
        size_t n = it->find('#');
        if (n != std::string::npos) {
            it->erase(n, std::string::npos); // удаляем комментарии
        }
        *it = trim(*it); // обрезаем пробелы в начале и конце строки


        if (it->empty() || std::all_of(it->begin(), it->end(), ::isspace)) {
            it = lines.erase(it); // удаляем строку, если она пустая или состоит только из пробелов
        }
        else {
            ++it;
        }
    }


    // Обрабатываем символы '\\' и ';'
    lines = processSpecialCharacters(lines);

    // Собираем строки обратно в текст, разделяя их символом новой строки
    processedText = "";
    for (const auto& line : lines)
        processedText = processedText + line + "\n";

    return processedText;
}

std::string NftablesParser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> NftablesParser::processSpecialCharacters(std::vector<std::string>& lines)
{
    std::vector<std::string> processedLines;

    for (size_t i = 0; i < lines.size(); ++i) {
        // Обрабатываем случай, когда строка оканчивается символом '\\'
        if (!lines[i].empty() && lines[i].back() == '\\') {
            lines[i].pop_back(); // удаляем символ '\\' с конца строки
            lines[i] += ' ' + lines[i + 1]; // объединяем строку с следующей
            lines.erase(lines.begin() + i + 1); // удаляем следующую строку
            --i; // возвращаемся назад, чтобы обработать текущую строку заново
            continue;
        }

        // Обрабатываем случай, когда в строке есть символ ';'
        size_t n = lines[i].find(';');
        if (n != std::string::npos) {
            // Если символ ';' не является последним в строке (не считая пробелов), разбиваем строку на две
            if (n < lines[i].size() - 1) {
                std::string firstPart = lines[i].substr(0, n);
                std::string secondPart = lines[i].substr(n + 1, std::string::npos);

                firstPart = trim(firstPart);
                secondPart = trim(secondPart);

                if (!firstPart.empty())
                    processedLines.push_back(firstPart);
                if (!secondPart.empty())
                    processedLines.push_back(secondPart);
            }
            else {
                lines[i].erase(n, 1);
                lines[i] = trim(lines[i]);
                if (!lines[i].empty())
                    processedLines.push_back(lines[i]);
            }
        }
        else {
            // Обработка случая, когда в строке есть символ '{'
            auto it = lines[i].rfind('{');
            if (it != std::string::npos && it == lines[i].find_last_not_of(" \t\n\v\f\r")) {
                lines[i].erase(it, 1);
                lines[i] = trim(lines[i]);
                if (!lines[i].empty())
                    processedLines.push_back(lines[i]);
                processedLines.push_back("{");
            }
            else {
                // Обработка случая, когда в строке есть символ '}'
                it = lines[i].find('}');
                if (it != std::string::npos && it < lines[i].find_first_not_of(" \t\n\v\f\r", it + 1)) {
                    std::string firstPart = lines[i].substr(0, it + 1);
                    std::string secondPart = lines[i].substr(it + 1, std::string::npos);

                    firstPart = trim(firstPart);
                    secondPart = trim(secondPart);

                    if (!firstPart.empty())
                        processedLines.push_back(firstPart);
                    if (!secondPart.empty())
                        processedLines.push_back(secondPart);
                }
                else {
                    processedLines.push_back(lines[i]);
                }
            }
        }
    }

    // Удаляем символ ';', если он последний в строке
    for (auto& line : processedLines) {
        if (!line.empty() && line.back() == ';') {
            line.pop_back(); // удаляем символ ';' с конца строки
            line = trim(line); // обрезаем пробелы в начале и конце строки
        }
    }

    return processedLines;
}

void NftablesParser::printTable(const Table &table)
{
    std::cout << "Table Name: " << table.name << "\n";
    std::cout << "Table Type: " << table.type << "\n";
    for (const Chain &chain : table.chains)
    {
        std::cout << "*****************************************************" << "\n";

        std::cout << "  Chain Name: " << chain.name << "\n";
        std::cout << "  Chain Type: " << chain.type << "\n";
        std::cout << "  Chain Hook: " << chain.hook << "\n";
        std::cout << "  Chain Priority: " << chain.priority << "\n";
        std::cout << "  Chain Policy: " << chain.policy << "\n";

        for (const Rule &rule : chain.rules) {
            std::cout << "---------------------------------" << "\n";

            std::cout << "    Rule Action: " << rule.action << "\n";
            std::cout << "    Rule Parameters: " << rule.parameters << "\n";
            std::cout << "    Rule Comment: " << rule.comment << "\n";
            std::cout << "    Rule Interface: " << rule.interface << "\n";
            std::cout << "    Rule Protocol: " << rule.protocol << "\n";
            std::cout << "    Rule Port: " << rule.port << "\n";
        }

    }
}

std::vector<std::string> NftablesParser::tokenize(const std::string &line, char delimiter) const
{
    std::vector<std::string> tokens;
    bool insideSingleQuotes = false;
    bool insideDoubleQuotes = false;
    std::string token;

    for (const auto& character : line) {
        if (character == delimiter && !insideSingleQuotes && !insideDoubleQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (character == '\"') {
            insideDoubleQuotes = !insideDoubleQuotes;
            token += character;
        }
        else if (character == '\'') {
            insideSingleQuotes = !insideSingleQuotes;
            token += character;
        }
        else {
            token += character;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

Chain NftablesParser::parseChain(const std::vector<std::string> &tokens)
{
    Chain chain;
    chain.name = tokens[1];
    return chain;
}

bool NftablesParser::isRule(const std::string& token) const
{
    const static std::unordered_set<std::string> ruleTokens = {"tcp", "iifname", "ct", "ip6", "ip"};

    return ruleTokens.find(token) != ruleTokens.end();
}

Rule NftablesParser::parseRule(const std::vector<std::string> &tokens)
{
    Rule rule;
    rule.interface = "any"; // устанавливаем значение по умолчанию для интерфейса

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "accept" || tokens[i] == "drop" || tokens[i] == "deny" || tokens[i] == "reject") {
            rule.action = tokens[i];
        }
        else if (tokens[i] == "iifname" && i + 1 < tokens.size()) {
            rule.interface = tokens[i + 1];
            i++; // skip next token, we already used it
        }
        else if ((tokens[i] == "tcp" || tokens[i] == "udp") && i + 2 < tokens.size() && tokens[i + 1] == "dport") {
            rule.protocol = tokens[i];
            rule.port = tokens[i + 2];
            i += 2; // skip next two tokens, we already used them
        }
        else if (tokens[i] == "comment" && i + 1 < tokens.size()) {
            rule.comment = tokens[i + 1];
            i++; // skip next token, we already used it
        }
        else if ((tokens[i] == "with" || tokens[i] == "type") && i + 1 < tokens.size()) {
            rule.parameters += "\"" + tokens[i + 1] + "\" ";
            i++; // skip next token, we already used it
        }
    }

    return rule;
}

void NftablesParser::parseChain(Chain &chain, const std::vector<std::string> &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "type" && i + 1 < tokens.size()) {
            chain.type = tokens[i + 1];
        }
        else if (tokens[i] == "hook" && i + 1 < tokens.size()) {
            chain.hook = tokens[i + 1];
        }
        else if (tokens[i] == "priority" && i + 1 < tokens.size()) {
            chain.priority = tokens[i + 1];
        }
        else if (tokens[i] == "policy" && i + 1 < tokens.size()) {
            chain.policy = tokens[i + 1];
        }
    }
}



