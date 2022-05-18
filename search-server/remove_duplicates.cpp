// Copyright 2022
// 16:50 18/05/2022

#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
    set<int> del_it;
    set<set<string>> doc_word;
    for (auto it = search_server.begin(); it != --search_server.end(); ) {
        set<string> check = search_server.word_in_documents_[*it];
        auto it_two = ++it;
        set<string> check_two = search_server.word_in_documents_[*it_two];
            if (!doc_word.count(check_two)) {
                if (check == check_two) {
                    del_it.insert(*it_two);
                } else {
                    doc_word.insert(check);
                }
            } else {
                 del_it.insert(*it_two);
            }
    }
    for (auto id : del_it) {
        cout << "Found duplicate document id " << id << endl;
        search_server.RemoveDocument(id);
    }
}
