// Copyright 2022
// search_server_s3_t1_v1.cpp
// search_server_s3_t1_v2.cpp
// search_server_s3_t3_v3.cpp
// search_server_s4_t2_solution.cpp
// 01:37 11.04.2022

#include <iostream>
#include <string>

#include "read_input_functions.h"
#include "paginator.h"
#include "request_queue.h"
#include "search_server.h"

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating << " }"s << endl;
}

int main() {
    {
        SearchServer search_server("and with"s);

        search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, {1, 2, 3});
        search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::ACTUAL, {1, 2, 8});
        search_server.AddDocument(4, "big dog cat Vladislav"s, DocumentStatus::ACTUAL, {1, 3, 2});
        search_server.AddDocument(5, "big dog hamster Borya"s, DocumentStatus::ACTUAL, {1, 1, 1});

        auto search_results = search_server.FindTopDocuments("curly dog"s);

        size_t page_size = 2;
        const auto pages = Paginate(search_results, page_size);

        for (auto page = pages.begin(); page != pages.end(); ++page) {
            cout << *page << endl;
            cout << "Page break"s << endl;
        }
    }

    {
        SearchServer search_server("and in at"s);
        RequestQueue request_queue(search_server);

        search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
        search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
        search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
        search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});

        for (int i = 0; i < 1439; ++i) {
            request_queue.AddFindRequest("empty request"s);
        }

        request_queue.AddFindRequest("curly dog"s);
        request_queue.AddFindRequest("big collar"s);
        request_queue.AddFindRequest("sparrow"s);

        cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    }
}
