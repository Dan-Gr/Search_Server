// Copyright 2022
// 01:41 11/04/2022

#include "request_queue.h"


    RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_in_class_(search_server) {
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        vector<Document> document = search_server_in_class_.FindTopDocuments(raw_query, status);
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

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        vector<Document> document = search_server_in_class_.FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
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

    int RequestQueue::GetNoResultRequests() const {
        return cointer_search_null_;
    }
