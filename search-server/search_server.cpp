// Copyright 2022
// 01:41 11/04/2022

#include "search_server.h"


    int SearchServer::GetDocumentId(int index) const {
        return id_documents_in_order_.at(index);;
    }


    SearchServer::SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text)) {
    }

    void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
                                   const vector<int>& ratings) {
        if (document_id < 0) {
            throw invalid_argument("Invalid ID"s);
        }
        if (documents_.count(document_id) != 0) {
            throw invalid_argument("The document with this ID has already been added"s);
        }
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        id_documents_in_order_.push_back(document_id);
    }

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int, DocumentStatus document_status, int) {
            return document_status == status;
        });
    }

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int SearchServer::GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
        tuple<vector<string>, DocumentStatus> result;
            if (document_id >= 0) {
                const Query query = ParseQuery(raw_query);
                vector<string> matched_words;
                for (const string& word : query.plus_words) {
                    if (word_to_document_freqs_.count(word) == 0) {
                        continue;
                    }
                    if (word_to_document_freqs_.at(word).count(document_id)) {
                        matched_words.push_back(word);
                    }
                }
                for (const string& word : query.minus_words) {
                    if (word_to_document_freqs_.count(word) == 0) {
                        continue;
                    }
                    if (word_to_document_freqs_.at(word).count(document_id)) {
                        matched_words.clear();
                        break;
                    }
                }
                result = {matched_words, documents_.at(document_id).status};
            }
    return result;
    }

    bool SearchServer::IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
            CheckForMoreMines(word);
        }
        return words;
    }

    int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
        return rating_sum;
    }

    SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
        bool is_minus = false;
        if (text.empty()) {
            throw invalid_argument("Empty request"s);
        }
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }

    SearchServer::Query SearchServer::ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            CheckForMoreMines(word);
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                    } else {
                        query.plus_words.insert(query_word.data);
                    }
            }
        }
        return query;
    }

    double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    void SearchServer::CheckForMoreMines(const string& word) {
        for (const char s : word) {
            if ((s == '-' && word.size() == 1) || (word[0] == '-' && word[1] == '-')) {
                throw invalid_argument("Added extra: -"s);
            }
            if (s >= '\0' && s < ' ') {
                throw invalid_argument("The document was not added because it contains special characters: "s + s);
            }
        }
    }
