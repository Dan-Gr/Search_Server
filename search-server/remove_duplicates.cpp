// Copyright 2022
// 13:36 20/05/2022

#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
    set<int> del_it;
    set<set<string>> doc_word;
    for (auto it = search_server.begin(); it != search_server.end(); it++) {
        set<string> check = search_server.ReturWord(*it);
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
