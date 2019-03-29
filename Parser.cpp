#include "Parser.h"

#include <cassert>
#include <fstream>
#include <sstream>

string Parser::parseQuote(string &line) {
    vector<unsigned long> loc;
    for (unsigned long i = 0; i < line.size(); ++i) {
        if (line[i] == '"') loc.push_back(i);
    }
    assert(loc.size() == 2);
    string res = line.substr(loc[0] + 1, loc[1] - loc[0] - 1);
    line = line.substr(loc[1] + 4);
    line = line.substr(0, line.size() - 1);
    string line2;
    for (const auto &i : line) {
        if (i != ',') line2 += i;
    }
    line = line2;
    return res;
}

pair<string, vector<DB>> Parser::parseLine(string line) {
    string infoSet = parseQuote(line);
    istringstream ss(line);
    DB cur;
    vector<DB> strat;
    while (ss >> cur) {
        strat.push_back(cur);
    }
    return make_pair(infoSet, strat);
}

unordered_map<string, vector<DB>> Parser::parse(const string &file) {
    unordered_map<string, vector<DB>> res;
    ifstream ifs(file);
    string line;
    while (getline(ifs, line)) {
        if (!line.empty()) {
            auto cline = parseLine(line);
            res[cline.first] = cline.second;
        }
    }
    return res;
}
