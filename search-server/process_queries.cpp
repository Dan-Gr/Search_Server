// Copyright 2022
// 16:32 11/06/2022

#include "process_queries.h"

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        std::vector<std::vector<Document>> result(queries.size());
        transform(std::execution::par,
            queries.begin(), queries.end(),
            result.begin(), 
            [&search_server] (const std::string& querie) {
                return search_server.FindTopDocuments(querie);
            });
        return result;
    }

std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        std::vector<std::vector<Document>> buffer_for_documents(queries.size());
        transform(std::execution::par,
            queries.begin(), queries.end(),
            buffer_for_documents.begin(), 
            [&search_server] (const std::string& querie) {
                return search_server.FindTopDocuments(querie);
            });
        // стоит поиграться с контейнерам для поиска наиболее эффективного
        vector<Document> result;
        for (std::vector<Document> check : buffer_for_documents) {
            result.insert(result.end(), check.begin(), check.end());
        }
        return result;
    }
