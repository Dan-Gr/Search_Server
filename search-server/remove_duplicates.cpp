// Copyright 2022
// 01:19 15/05/2022

#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
    // vector<std::vector<int>::iterator> del_it;
    set<int> del_it;
    for (auto it = search_server.begin(); it != search_server.end(); ++it) {
        set<string> check = search_server.word_in_documents_[*it];
        if (it != search_server.end() - 1) {
        for (auto it_two = it + 1; it_two != search_server.end(); ++it_two) {
            set<string> check_two = search_server.word_in_documents_[*it_two];
            if (check == check_two) {
                del_it.insert(*it_two);
            }
        }
        }
    }
    for (auto id : del_it) {
        cout << "Found duplicate document id " << id << endl;
        search_server.RemoveDocument(id);
    }
}
