// Copyright 2022
// 16:39 11/06/2022

#include "string_processing.h"

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    std::for_each(text.begin(), text.end(), [&word, &words] (const char& c) {
            if (c == ' ') {
                if (!word.empty()) {
                    words.push_back(word);
                    word.clear();
                }
            } else {
                word += c;
            }
        });
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    if (!str.empty()) {
        int64_t pos = str.find_first_not_of(" ");
        const int64_t pos_end = str.npos;
        if (pos != pos_end) {
            str.remove_prefix(pos);
            while (!str.empty()) {
                int64_t space = str.find(' ');
                if (space == -1) {
                    result.push_back(str);
                    break;
                }
                result.push_back(str.substr(0, space));
                str.remove_prefix(space);
                for ( ; str.find(' ') == 0; ) {
                    str.remove_prefix(1);
                }
            }
        }
    }
    return result;
}
