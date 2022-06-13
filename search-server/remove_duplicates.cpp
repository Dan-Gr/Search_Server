// Copyright 2022
// 16:35 11/06/2022

#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::set<int> del_it;
    std::set<std::set<std::string>> doc_word;
    for (auto it = search_server.begin(); it != search_server.end(); it++) {
        std::set<std::string> check = search_server.ReturWord(*it);
        if (!doc_word.count(check)) {
            doc_word.insert(check);
        } else {
             del_it.insert(*it);
        }
    }
    for (auto id : del_it) {
        cout << "Found duplicate document id " << id << endl;
        search_server.RemoveDocument(id);
    }
}
