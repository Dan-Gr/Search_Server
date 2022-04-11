// Copyright 2022
// 01:41 11/04/2022

#pragma once

#include <deque>
#include <vector>
#include <string>

#include "search_server.h"

using std::vector;
using std::string;
using std::deque;

class RequestQueue {
 public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        vector<Document> document = search_server_in_class_.FindTopDocuments(raw_query, document_predicate);
        QueryResult buffer;
        buffer.search_result = document;
        if (document.empty()) {
            buffer.status = false;
            ++cointer_search_null_;
        } else {
            buffer.status = true;
        }
        ++cointer_search;
        requests_.push_back(buffer);

        if (cointer_search > min_in_day_) {
            QueryResult del = requests_.front();
            if (del.status == false) {
                --cointer_search_null_;
            }
            --cointer_search;
            requests_.pop_back();
        }
        return document;
    }

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);

    vector<Document> AddFindRequest(const string& raw_query);

    int GetNoResultRequests() const;

 private:
    const SearchServer& search_server_in_class_;

    struct QueryResult {
        vector<Document> search_result;
        bool status;
    };

    deque<QueryResult> requests_;
    static const int min_in_day_ = 1440;
    int cointer_search_null_ = 0;
    int cointer_search = 0;
};
