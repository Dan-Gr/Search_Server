// Copyright 2022
// 16:50 18/05/2022

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
        ++counter_search_null_;
    } else {
        buffer.status = true;
    }
    ++counter_search;
    requests_.push_back(buffer);

    if (counter_search > min_in_day_) {
        QueryResult del = requests_.front();
        if (del.status == false) {
            --counter_search_null_;
        }
        --counter_search;
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
        ++counter_search_null_;
    } else {
        buffer.status = true;
    }
    ++counter_search;
    requests_.push_back(buffer);

    if (counter_search > min_in_day_) {
        QueryResult del = requests_.front();
        if (del.status == false) {
            --counter_search_null_;
        }
        --counter_search;
        requests_.pop_back();
    }
    return document;
}

int RequestQueue::GetNoResultRequests() const {
    return counter_search_null_;
}
