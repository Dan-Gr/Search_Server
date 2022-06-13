// Copyright 2022
// 13:36 20/05/2022

#include "search_server.h"
using namespace std;

void SearchServer::RemoveDocument(int document_id) {
    return RemoveDocument(std::execution::seq, document_id);
}

void SearchServer::RemoveDocument(const execution::sequenced_policy&, int document_id) {
    for_each(std::execution::seq, id_document_and_word_freqs_.at(document_id).begin(), id_document_and_word_freqs_.at(document_id).end(),
    [&, document_id] (const auto& doc_and_freqs) {
            word_to_document_freqs_.at(doc_and_freqs.first).erase(document_id);
    });
    documents_.erase(document_id);
    id_documents_in_order_.erase(document_id);
    id_document_and_word_freqs_.erase(document_id);
}

void SearchServer::RemoveDocument(const execution::parallel_policy&, int document_id) {
    vector<string> buffer(id_document_and_word_freqs_.at(document_id).size());
    transform(std::execution::par,
        id_document_and_word_freqs_.at(document_id).begin(), id_document_and_word_freqs_.at(document_id).end(),
        buffer.begin(), [] (const auto& doc_and_freqs) {
            return doc_and_freqs.first;
        });
    for_each(std::execution::par, buffer.begin(), buffer.end(), [this, &document_id] (const string& virb) {
        word_to_document_freqs_.at(virb).erase(document_id);
    });
    documents_.erase(document_id);
    id_documents_in_order_.erase(document_id);
    id_document_and_word_freqs_.erase(document_id);
}

set<string> SearchServer::ReturWord(const int id) {
    set <string> check;
    for (auto [word, freqs] : id_document_and_word_freqs_[id]) {
        check.insert(static_cast<string>(word));
    }
    return check;
}

std::set<int>::iterator SearchServer::begin() {
        return id_documents_in_order_.begin();
    }

std::set<int>::iterator SearchServer::end() {
        return id_documents_in_order_.end();
    }

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static map<string_view, double> word_frequencies_;
    if (!word_frequencies_.empty()) {
        word_frequencies_.clear();
    }
    word_frequencies_ = id_document_and_word_freqs_.at(document_id);
    return word_frequencies_;
}

SearchServer::SearchServer(const string_view& stop_words_text)
    : SearchServer(SplitIntoWordsView(stop_words_text)) {
}

SearchServer::SearchServer(const string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text)) {
}

void SearchServer::AddDocument(int document_id, const string_view& document, DocumentStatus status,
                                   const vector<int>& ratings) {
    if (document_id < 0 || documents_.count(document_id) != 0) {
        throw invalid_argument("Invalid ID or The document with this ID has already been added"s);
    }
    const vector<string_view> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const string_view& word : words) {
        // сохраняем копию преобразованную в string
        auto point = save_all_words.insert(save_all_words.begin(), static_cast<string>(word));
        word_to_document_freqs_[*point][document_id] += inv_word_count;
        id_document_and_word_freqs_[document_id][*point] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    id_documents_in_order_.insert(document_id);
}

vector<Document> SearchServer::FindTopDocuments(const string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::seq, raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::seq, raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::par, raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

vector<Document> SearchServer::FindTopDocuments(const string_view& raw_query) const {
    return FindTopDocuments(std::execution::seq, raw_query, DocumentStatus::ACTUAL);
}


vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy&, const string_view& raw_query) const {
    return FindTopDocuments(std::execution::seq, raw_query, DocumentStatus::ACTUAL);
}

vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy&, const string_view& raw_query) const {
    return FindTopDocuments(std::execution::par, raw_query, DocumentStatus::ACTUAL);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy&, const string_view& raw_query, int document_id) const {
    vector<string_view> matched_words;
    if (!id_documents_in_order_.count(document_id)) {
        throw out_of_range("id");
    } else {
        QueryVector query = ParseQueryVector(raw_query);
        auto check = find_if(std::execution::seq, query.minus_words.begin(), query.minus_words.end(),
            [this, &document_id] (const string_view& word) {
                return word_to_document_freqs_.at(word).count(document_id);
            });
        if (check != query.minus_words.end()) {
            return tuple(matched_words, documents_.at(document_id).status);
        }
        matched_words.reserve(query.plus_words.size());
        for (const string_view& word : query.plus_words) {
            // без !count(matched_words.begin(), matched_words.end(), word) берем лишние варианты
            if (word_to_document_freqs_.at(word).count(document_id) && !count(std::execution::seq, matched_words.begin(), matched_words.end(), word)) {
                auto last = const_cast<list<string>&>(save_all_words).insert(save_all_words.begin(), static_cast<string>(word));
                matched_words.insert(matched_words.end(), *last);
            }
        }
    }
    return tuple(matched_words, documents_.at(document_id).status);;
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy&, const string_view& raw_query, int document_id) const {
    vector<string_view> matched_words;
    if (!id_documents_in_order_.count(document_id)) {
        throw out_of_range("id");
    } else {
        QuerySet query = ParseQuerySet(raw_query);
        auto check = find_if(std::execution::seq, query.minus_words.begin(), query.minus_words.end(),
            [this, &document_id] (const string_view word) {
                return word_to_document_freqs_.at(word).count(document_id);
            });
            if (check != query.minus_words.end()) {
                return tuple(matched_words, documents_.at(document_id).status);
            }
        matched_words.reserve(query.plus_words.size());
        for (const string_view& word : query.plus_words) {
            if (word_to_document_freqs_.at(word).count(document_id)) {
                auto last = const_cast<list<string>&>(save_all_words).insert(save_all_words.begin(), static_cast<string>(word));
                matched_words.insert(matched_words.end(), *last);
            }
        }
    }
    return tuple(matched_words, documents_.at(document_id).status);
}

void SearchServer::SaveWords(vector<string_view>& words, string_view& word) {
    auto last = save_all_words.insert(save_all_words.end(), static_cast<string>(word));
    words.push_back(*last);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const string_view& raw_query, int document_id) const {
    return MatchDocument(std::execution::seq, raw_query, document_id);
}

bool SearchServer::IsStopWord(const string_view word) const {
    return stop_words_.count(word);
}

vector<string_view> SearchServer::SplitIntoWordsNoStop(const string_view& text) const {
    vector<string_view> words;
    for (const string_view word : SplitIntoWordsView(text)) {
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
    } else if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    return {text, is_minus, IsStopWord(text)};
}

SearchServer::QueryVector SearchServer::ParseQueryVector(const string_view& text) const {
    QueryVector query;
    for (const string_view& word : SplitIntoWordsView(text)) {
        if (word[0] == '-' && word[1] == '-') {
            throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
        }
        for (const char& sign : word) {
            if ((sign >= '\0' && sign < ' ') || (sign == '-' && word.size() == 1)) {
                throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
            }
        }
        const QueryWord query_word = ParseQueryWord(static_cast<string>(word));
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.push_back(query_word.data);
            }
            if (!query_word.is_minus) {
                query.plus_words.push_back(query_word.data);
            }
        }
    }
    return query;
}

SearchServer::QuerySet SearchServer::ParseQuerySet(const string_view& text) const {
    QuerySet query;
    for (const string_view word : SplitIntoWordsView(text)) {
        if (word[0] == '-' && word[1] == '-') {
            throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
        }
        for (const char& sign : word) {
            if ((sign >= '\0' && sign < ' ') || (sign == '-' && word.size() == 1)) {
                throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
            }
        }
        const QueryWord query_word = ParseQueryWord(static_cast<string>(word));
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(static_cast<string>(query_word.data));
            }
            if (!query_word.is_minus) {
                query.plus_words.insert(static_cast<string>(query_word.data));
            }
        }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string_view& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

void SearchServer::CheckForMoreMines(const string_view& word) {
    if (word[0] == '-' && word[1] == '-') {
        throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
    }
    for (const char& sign : word) {
        if ((sign >= '\0' && sign < ' ') || (sign == '-' && word.size() == 1)) {
            throw invalid_argument("Added extra '-' or The document was not added because it contains special characters: "s);
        }
    }
}


