// готов правиить до победного конца, мне - это только в удовольствие)
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <numeric>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ALLOWED_ERROR = 1e-6;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char& c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
 public:
    int GetDocumentId(int index) const {
        if (!id_documents_in_order_.at(index)) {
            throw out_of_range("Out of range"s);
            } else {
                return id_documents_in_order_.at(index);
                }
        }

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  {
            for_each(stop_words_.begin(), stop_words_.end(), CheckForMoreMines);
        }

    explicit SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text)) {
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status,
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

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
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

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int, DocumentStatus document_status, int) {
            return document_status == status;
        });
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
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

 private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    vector<int> id_documents_in_order_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
            CheckForMoreMines(word);
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings) {
        int rating_sum = 0;
        if (!ratings.empty()) {
            rating_sum = accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
        }
        return rating_sum;
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
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

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
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

    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
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

    static void CheckForMoreMines(const string& words) {
            for (const char s : words) {
                if ((s == '-' && words.size() == 1) || (words[0] == '-' && words[1] == '-')) {
                    throw invalid_argument("Added extra: -"s);
                }
                if (s >= '\0' && s < ' ') {
                    string text = "The document was not added because it contains special characters: "s + s;
                    throw invalid_argument(text);
                }
            }
    }
};



// ==================== для примера =========================

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating << " }"s << endl;
}

void PrintMatchDocumentResult(int document_id, const vector<string>& words, DocumentStatus status) {
    cout << "{ "s
         << "document_id = "s << document_id << ", "s
         << "status = "s << static_cast<int>(status) << ", "s
         << "words ="s;
    for (const string& word : words) {
        cout << ' ' << word;
    }
    cout << "}"s << endl;
}

void AddDocument(SearchServer& search_server, int document_id, const string& document, DocumentStatus status,
                 const vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const exception& e) {
        cout << "Error adding a document "s << document_id << ": "s << e.what() << endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const string& raw_query) {
    cout << "Search results for: "s << raw_query << endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const exception& e) {
        cout << "Search error: "s << e.what() << endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const string& query) {
    try {
        cout << "Matching documents on request: "s << query << endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = search_server.GetDocumentId(index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    } catch (const exception& e) {
        cout << "Error in matching documents to a request "s << query << ": "s << e.what() << endl;
    }
}

int main() {
    SearchServer search_server("and in a"s);

    AddDocument(search_server, 1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    AddDocument(search_server, 1, "a fluffy dog and a fashionable collar"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, -1, "a fluffy dog and a fashionable collar"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 3, "big dog star\x12ling eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    AddDocument(search_server, 4, "big dog starling eugene"s, DocumentStatus::ACTUAL, {1, 1, 1});

    FindTopDocuments(search_server, "fluffy -dog"s);
    FindTopDocuments(search_server, "fluffy --cat"s);
    FindTopDocuments(search_server, "fluffy -"s);

    MatchDocuments(search_server, "fluffy dog"s);
    MatchDocuments(search_server, "fashionable -cat"s);
    MatchDocuments(search_server, "fashionable --dog"s);
    MatchDocuments(search_server, "fluffy - tail"s);

    cout << "num_1: "s << search_server.GetDocumentId(1) << endl;
}
