// Copyright 2022
// 16:40 11/06/2022

#pragma once

#include <vector>
#include <string>
#include <set>
#include <execution>
#include <algorithm>
#include <string_view>

using std::set;
using std::vector;
using std::string;
using std::string_view;
using std::less;

vector<string> SplitIntoWords(const string& text);
vector<std::string_view> SplitIntoWordsView(std::string_view str);

template <typename StringContainer>
set<string, less<>> MakeUniqueNonEmptyStrings(const StringContainer strings) {
    set<string, less<>> non_empty_strings;
    for (const string_view& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(static_cast<string>(str));
        }
    }
    return non_empty_strings;
}

