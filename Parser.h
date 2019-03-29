#ifndef CFR_PARSER_H
#define CFR_PARSER_H

#include <unordered_map>
#include "Node.h"

using namespace std;

class Parser {
public:

    static string parseQuote(string &line);

    static pair<string, vector<DB>> parseLine(string line);

    static unordered_map<string, vector<DB>> parse(const string &file);

    Parser() = delete;
};


#endif //CFR_PARSER_H
