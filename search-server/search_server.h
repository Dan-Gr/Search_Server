// Copyright 2022
// 23:06 11/04/2022

#pragma once

#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>
#include <set>

using namespace std;

#include "string_processing.h"
#include "document.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ALLOWED_ERROR = 1e-6;

class SearchServer {
 public:
    int GetDocumentId(int index) const;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);

    explicit SearchServer(const string& stop_words_text);

    void AddDocument(int document_id, const string& document, DocumentStatus status,
                                   const vector<int>& ratings);

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const;

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const string& raw_query) const;
    int GetDocumentCount() const;
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const;

 private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    const set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    vector<int> id_documents_in_order_;

    bool IsStopWord(const string& word) const;
    vector<string> SplitIntoWordsNoStop(const string& text) const;
    static int ComputeAverageRating(const vector<int>& ratings);

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const;

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const;

    double ComputeWordInverseDocumentFreq(const string& word) const;

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;

    static void CheckForMoreMines(const string& word);
};

template <typename StringContainer>
    SearchServer::SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  {
            for_each(stop_words_.begin(), stop_words_.end(), CheckForMoreMines);
    }

template <typename DocumentPredicate>
vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
    vector<Document> result;
    const Query query = ParseQuery(raw_query);
    result = FindAllDocuments(query, document_predicate);
    sort(result.begin(), result.end(), [](const Document& lhs, const Document& rhs) {
        if (abs(lhs.relevance - rhs.relevance) < ALLOWED_ERROR) {
             return lhs.rating > rhs.rating;
        } else {
            return lhs.relevance > rhs.relevance;
        }
    });
    if (result.size() > MAX_RESULT_DOCUMENT_COUNT) {
        result.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return result;
    }

template <typename DocumentPredicate>
    vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
    map<int, double> document_to_relevance;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
    const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
    for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
        const auto& document_data = documents_.at(document_id);
        if (document_predicate(document_id, document_data.status, document_data.rating)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }
    }

    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }
    return matched_documents;
}