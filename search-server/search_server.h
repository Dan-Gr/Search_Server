// Copyright 2022
// 14:45 15/06/2022

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
#include <execution>
#include <list>
#include <string_view>

using namespace std;

#include "string_processing.h"
#include "document.h"
#include "log_duration.h"
#include "concurrent_map.h"


const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ALLOWED_ERROR = 1e-6;

class SearchServer {
 public:
    int GetDocumentId(int index) const;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);

    explicit SearchServer(const string_view& stop_words_text);

    explicit SearchServer(const string& stop_words_text);

    void AddDocument(int document_id, const string_view& document, DocumentStatus status,
                                const vector<int>& ratings);

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string_view& raw_query, DocumentPredicate document_predicate) const;

    vector<Document> FindTopDocuments(const string_view& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const string_view& raw_query) const;

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query, DocumentPredicate document_predicate) const;

    vector<Document> FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query) const;

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query, DocumentPredicate document_predicate) const;

    vector<Document> FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query) const;

    int GetDocumentCount() const;

    tuple<vector<string_view>, DocumentStatus> MatchDocument(const string_view& raw_query, int document_id) const;
    tuple<vector<string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy&, const string_view& raw_query, int document_id) const;
    tuple<vector<string_view>, DocumentStatus> MatchDocument(const std::execution::sequenced_policy&, const string_view& raw_query, int document_id) const;

    const map<string_view, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);
    void RemoveDocument(const std::execution::parallel_policy&, int document_id);
    void RemoveDocument(const std::execution::sequenced_policy&, int document_id);
    set<string> ReturWord(const int id);

    std::set<int>::iterator begin();
    std::set<int>::iterator end();

 private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    list<string> save_all_words;
    const set<string, less<>> stop_words_;

    map<string_view, map<int, double>> word_to_document_freqs_;
    map<int, map<string_view, double>> id_document_and_word_freqs_;
    map<int, DocumentData> documents_;
    set<int> id_documents_in_order_;

    bool IsStopWord(const string_view word) const;
    vector<string_view> SplitIntoWordsNoStop(const string_view& text) const;
    static int ComputeAverageRating(const vector<int>& ratings);
    void SaveWords(vector<string_view>& words, string_view& word);

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const;

    struct QuerySet {
        set<string, less<>> plus_words;
        set<string, less<>> minus_words;
    };

    struct QueryVector {
        vector<string> plus_words;
        vector<string> minus_words;
    };

    QuerySet ParseQuerySet(const string_view& text) const;
    QueryVector ParseQueryVector(const string_view& text) const;

    double ComputeWordInverseDocumentFreq(const string_view& word) const;

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const std::execution::sequenced_policy&, const QuerySet& query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const std::execution::parallel_policy&, const QueryVector& query, DocumentPredicate document_predicate) const;

    static void CheckForMoreMinuses(const string_view& word);
};

template <typename StringContainer>
    SearchServer::SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  {
            for_each(stop_words_.begin(), stop_words_.end(), CheckForMoreMinuses);
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindTopDocuments(const string_view& raw_query, DocumentPredicate document_predicate) const {
    return FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query, DocumentPredicate document_predicate) const {
    vector<Document> result;
    const QuerySet query = ParseQuerySet(raw_query);
    result = FindAllDocuments(std::execution::seq, query, document_predicate);
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
vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query, DocumentPredicate document_predicate) const {
    vector<Document> result;
    QueryVector query = ParseQueryVector(raw_query);

    sort(std::execution::par, query.plus_words.begin(), query.plus_words.end());
    auto last = unique(std::execution::par, query.plus_words.begin(), query.plus_words.end());
    query.plus_words.erase(last, query.plus_words.end());

    result = FindAllDocuments(std::execution::par, query, document_predicate);
    sort(std::execution::par, result.begin(), result.end(), [](const Document& lhs, const Document& rhs) {
        if (!(abs(lhs.relevance - rhs.relevance) < ALLOWED_ERROR)) {
            return lhs.relevance > rhs.relevance;
             return lhs.rating > rhs.rating;
        } else {
            return lhs.rating > rhs.rating;
        }
    });
    if (result.size() > MAX_RESULT_DOCUMENT_COUNT) {
        result.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return result;
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindAllDocuments(const std::execution::sequenced_policy&, const QuerySet& query, DocumentPredicate document_predicate) const {
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
    for_each(std::execution::seq, query.minus_words.begin(), query.minus_words.end(),
        [this, &document_to_relevance](const string_view& word) {
            if (word_to_document_freqs_.count(word)) {
                for (const auto [document_id, freqs] : word_to_document_freqs_.at(word)) {
                    document_to_relevance.erase(document_id);
                }
            }
            });
    vector<Document> matched_documents;
    for_each(std::execution::seq, document_to_relevance.begin(), document_to_relevance.end(),
        [this, &matched_documents] (const auto& pair) {
            matched_documents.insert(matched_documents.end(), {
                pair.first, pair.second, documents_.at(pair.first).rating
            });
        });
    return matched_documents;
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy&, const QueryVector& query, DocumentPredicate document_predicate) const {
        size_t experimental = 128;
        ConcurrentMap<int, double> document_to_relevance_with_mt(experimental);
        for_each(std::execution::par, query.plus_words.begin(), query.plus_words.end(),
            [this, &document_to_relevance_with_mt, &document_predicate](const string_view& word) {
                const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    const auto& document_data = documents_.at(document_id);
                    if (word_to_document_freqs_.count(word) && document_predicate(document_id, document_data.status, document_data.rating)) {
                        document_to_relevance_with_mt[document_id].ref_to_value += term_freq * inverse_document_freq;
                    }
                }
            });

        map<int, double> document_to_relevance(document_to_relevance_with_mt.BuildOrdinaryMap());
        mutex met_for_erase;
        for_each(std::execution::seq, query.minus_words.begin(), query.minus_words.end(),
            [this, &document_to_relevance, &met_for_erase](const string_view& word) {
                if (word_to_document_freqs_.count(word)) {
                    lock_guard<mutex> guard(met_for_erase);
                    for (const auto [document_id, freqs] : word_to_document_freqs_.at(word)) {
                        document_to_relevance.erase(document_id);
                    }
                }
            });

        vector<Document> matched_documents;
        mutex met_for_insert;
        matched_documents.reserve(document_to_relevance.size());
        for_each(std::execution::par, document_to_relevance.begin(), document_to_relevance.end(),
            [this, &matched_documents, &met_for_insert](const auto& pair) {
                lock_guard<mutex> guard(met_for_insert);
                matched_documents.insert(matched_documents.end(), { pair.first, pair.second, documents_.at(pair.first).rating });
            });
        return matched_documents;
}
