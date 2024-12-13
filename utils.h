#ifndef __DOMAINUS_UTILS_H__
#define __DOMAINUS_UTILS_H__

#include <algorithm> 
#include <cctype>
#include <locale>
#include <string>
#include <list>
#include <map>

using String = std::string;
using StringList = std::list<String>;
using StringMap = std::map<String, String>;


// trim from start (in place)
inline void ltrim(String &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(String &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

#endif//__DOMAINUS_UTILS_H__
