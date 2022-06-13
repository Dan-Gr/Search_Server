#include "process_queries.h"
#include "search_server.h"

#include <execution>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating << " }"s << endl;
}

int main() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "white cat and yellow hat"s,
            "curly cat curly tail"s,
            "nasty dog with big eyes"s,
            "nasty pigeon john"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }


    cout << "ACTUAL by default:"s << endl;
    // последовательная версия
    for (const Document& document : search_server.FindTopDocuments("curly nasty cat"s)) {
        PrintDocument(document);
    }
    cout << "BANNED:"s << endl;
    // последовательная версия
    for (const Document& document : search_server.FindTopDocuments(execution::seq, "curly nasty cat"s, DocumentStatus::BANNED)) {
        PrintDocument(document);
    }

    cout << "Even ids:"s << endl;
    // параллельная версия
    for (const Document& document : search_server.FindTopDocuments(execution::par, "curly nasty cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }

    return 0;
} 



































































// // Copyright 2022
// // search_server_s3_t1_v1.cpp
// // search_server_s3_t1_v2.cpp
// // search_server_s3_t3_v3.cpp
// // search_server_s4_t2_solution.cpp
// // 23:06 11/04/2022

// #include <iostream>
// #include <string>
// #include <string_view>
// #include <cassert>

// #include "read_input_functions.h"
// #include "paginator.h"
// #include "request_queue.h"
// #include "search_server.h"

// void PrintDocument(const Document& document) {
//     cout << "{ "s
//          << "document_id = "s << document.id << ", "s
//          << "relevance = "s << document.relevance << ", "s
//          << "rating = "s << document.rating << " }"s << endl;
// }
// // 
// // int main() {
// //     {
// //         string_view test_one = "and with"sv;
// //         string test_two = "and with"s;
// //         vector<string> test_vector_s = {"and", "with"};
// //         vector<string_view> test_vector_sv = {"and", "with"};

// //         // тест конструкторов
// //         SearchServer search_server(test_one);
// //         {
// //             SearchServer search_server_two(test_two);
// //             SearchServer search_server_three(test_vector_s);
// //             SearchServer search_server_four(test_vector_sv);
// //         }
// //         search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, {7, 2, 7});
// //         assert(search_server.GetDocumentCount() == 1);
// //         search_server.AddDocument(2, "funny pet with curly hair"sv, DocumentStatus::ACTUAL, {1, 2, 3});
// //         assert(search_server.GetDocumentCount() == 2);
// //         search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::ACTUAL, {1, 2, 8});
// //         assert(search_server.GetDocumentCount() == 3);
// //         search_server.AddDocument(4, "big dog cat Vladislav"sv, DocumentStatus::ACTUAL, {1, 3, 2});
// //         assert(search_server.GetDocumentCount() == 4);
// //         search_server.AddDocument(5, "Eliminates all except the first element from"s, DocumentStatus::ACTUAL, {6, 2, 7});
// //         assert(search_server.GetDocumentCount() == 5);
// //         search_server.AddDocument(6, "in the range in such a way that elements"sv, DocumentStatus::ACTUAL, {4, 2, 5});
// //         assert(search_server.GetDocumentCount() == 6);
// //         search_server.AddDocument(7, "While the signature does not need"s, DocumentStatus::ACTUAL, {2, 5, 2});
// //         assert(search_server.GetDocumentCount() == 7);
// //         search_server.AddDocument(8, "A ForwardIt to the new end of the range."sv, DocumentStatus::ACTUAL, {2, 4, 4});
// //         assert(search_server.GetDocumentCount() == 8);
// //         search_server.AddDocument(9, "Returns the number of elements in the range "s, DocumentStatus::ACTUAL, {4, 4, 4});
// //         assert(search_server.GetDocumentCount() == 9);
// //         search_server.AddDocument(10, "the range of elements to examine"sv, DocumentStatus::ACTUAL, {1, 1, 1});
// //         assert(search_server.GetDocumentCount() == 10);
// //         search_server.AddDocument(11, "exactly last"s, DocumentStatus::ACTUAL, {1, 1, 1});
// //         assert(search_server.GetDocumentCount() == 11);
// //         search_server.AddDocument(12, "number of elements satisfying the condition"sv, DocumentStatus::ACTUAL, {1, 1, 1});
// //         assert(search_server.GetDocumentCount() == 12);


// //     const string query = "elements range element except -from"s;
// //     const string_view query_sv = "elements range element except -from"sv;
// //     // тесты MatchDocument
// //     {
// //         const auto [words, status] = search_server.MatchDocument(query, 1);
// //         cout << words.size() << " words for document 1 s"s << endl;
// //         // 0
// //     }
// //     {
// //         const auto [words, status] = search_server.MatchDocument(query_sv, 1);
// //         cout << words.size() << " words for document 1 sv"s << endl;
// //         // 0
// //     }
// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::seq, query, 5);
// //         cout << words.size() << " words for document 5 s"s << endl;
// //         // 0
// //     }
// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::seq, query_sv, 5);
// //         cout << words.size() << " words for document 5 sv seq"s << endl;
// //         // 0
// //     }

// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::par, query_sv, 5);
// //         cout << words.size() << " words for document 5 sv par"s << endl;
// //         // 0
// //     }  
// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::seq, query, 6);
// //         cout << words.size() << " words for document 6 s"s << endl;
// //         // 2
// //     }  
// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::seq, query_sv, 6);
// //         cout << words.size() << " words for document 6 sv seq"s << endl;
// //         // 2
// //     }  
// //     {
// //         const auto [words, status] = search_server.MatchDocument(execution::par, query_sv, 6);
// //         cout << words.size() << " words for document 6 sv par"s << endl;
// //         // 2
// //     }  
// //     {
// //         const auto [words, status] = search_server.MatchDocument(query_sv, 6);
// //         cout << words.size() << " words for document 6 s"s << endl;
// //         // 2
// //     }  
// //     {
// //         const auto [words, status] = search_server.MatchDocument(query, 6);
// //         cout << words.size() << " words for document 6 sv"s << endl;
// //         // 2
// //     }  
// //     cout << endl;
// //     // тесты FindTopDocuments
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query);
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query_sv);
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     cout << endl;
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query, DocumentStatus::ACTUAL);
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query_sv);
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     cout << endl;
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0; });
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     {
// //         vector<Document> test_find = search_server.FindTopDocuments(query, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0; });
// //         for (auto& doc : test_find) {
// //             PrintDocument(doc);
// //         }
// //     }
// //     cout << endl;
// //     // тесты GetWordFrequencies
// //     {
// //         auto check = search_server.GetWordFrequencies(12);
// //         for (auto [word, num] : check) {
// //             cout << "word: " << word << ", num = " << num << endl;
// //         }
// //     }
// //     }

// // }


// // -------- Начало модульных тестов поисковой системы ----------

// // Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
// void TestExcludeStopWordsFromAddedDocumentContent() {

//         string_view test_one = "and with"sv;
//         string test_two = "and with"s;
//         string test_three = "from";
//         vector<string> test_vector_s = {"and", "with"};
//         vector<string_view> test_vector_sv = {"and", "with"};
//         SearchServer search_server_two(test_two);
//         SearchServer search_server_three(test_vector_s);
//         SearchServer search_server_four(test_vector_sv);
//         SearchServer search_server_five(test_three);
//         SearchServer search_server_six("from and"s);
//         SearchServer server("from"s);

//     const int doc_id = 42;
//     const string content = "cat in the city"s;
//     const vector<int> ratings = {1, 2, 3};

//     // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
//     // находит нужный документ
//     {
//         // SearchServer server("from"s);
//         // server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
//         // const auto found_docs = server.FindTopDocuments("in"s);
//         // assert(found_docs.size() == 1);
//         // const Document& doc0 = found_docs[0];
//         // assert(doc0.id == doc_id);
//     }

//     // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
//     // возвращает пустой результат
//     {
//         // SearchServer server("in the"s);
//         // server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
//         // assert(server.FindTopDocuments("in"s).empty());
//     }
// }

// // цель теста: проверить, список возвращаемых из документа слов, заданных в поиске, с учетом наличия минус слов

// void TestMatch() {
//     int id_one = 1;
//     int id_two = 2;
//     int id_three = 3;
//     int id_four = 4;
//     string doc_one = "small cat in the small city"s;
//     string doc_two = "dog in the town"s;
//     string doc_three = "cat printer table in for car seven apple"s;
//     string doc_four = "test school never again song in with"s;
//     vector<int> rating_one = {-10, 0, 8};
//     vector<int> rating_two = {-10, -6, -1};
//     vector<int> rating_three = {1, 4, 12};
//     vector<int> rating_four = {0};
//     string search_query = "cat printer test"s;


//     // вводные: есть дублируемые в тексте документа слова
//     // id задан верно
//     // поисковый запрос содержит слова содержащиеся в тексте документа
//     // минус слов нет
//     // документ документов дается несколько
//     // все документы актуальны
//     {
//         SearchServer server("from"s);
//         server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//         server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//         server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_one);
//         server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//         tuple<vector<string_view>, DocumentStatus> test_match_one = server.MatchDocument(search_query, id_one);
//         tuple<vector<string_view>, DocumentStatus> test_match_two = server.MatchDocument(search_query, id_two);
//         tuple<vector<string_view>, DocumentStatus> test_match_three = server.MatchDocument(search_query, id_three);
//         tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument(search_query, id_four);
//         cout << "cat = " << get<0>(test_match_one)[0] << endl;
//         // assert(get<0>(test_match_one)[0] == "cat");
//         assert(get<0>(test_match_one).size() == 1);

//         assert(get<0>(test_match_two).size() == 0);
//         cout << "cat = " << get<0>(test_match_three)[0] << endl;
//         assert(get<0>(test_match_three)[0] == "cat");
//         cout << "printer = " << get<0>(test_match_three)[1] << endl;
//         assert(get<0>(test_match_three)[1] == "printer");
//         assert(get<0>(test_match_three).size() == 2);

//         assert(get<0>(test_match_four)[0] == "test");
//         assert(get<0>(test_match_four).size() == 1);
//     }

//     // вводные: есть дублируемые в тексте документа слова
//     // id задан неверно
//     // поисковый запрос содержит слова содержащиеся в тексте документа
//     // минус слов нет
//     // документов дается несколько
    
//     {
//         SearchServer server_one("from"s);
//         server_one.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//         server_one.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//         server_one.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_one);
//         server_one.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//         tuple<vector<string_view>, DocumentStatus> test_match_one_server_one = server_one.MatchDocument(search_query, id_one);
//         tuple<vector<string_view>, DocumentStatus> test_match_two_server_one = server_one.MatchDocument(search_query, id_two);
//         tuple<vector<string_view>, DocumentStatus> test_match_three_server_one = server_one.MatchDocument(search_query, id_three);
//         // tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument(search_query, 89);
//         cout << "cat = " << get<0>(test_match_one_server_one)[0] << endl;
//         assert(get<0>(test_match_one_server_one)[0] == "cat");
//         assert(get<0>(test_match_one_server_one).size() == 1);

//         assert(get<0>(test_match_two_server_one).size() == 0);

//         assert(get<0>(test_match_three_server_one)[0] == "cat");
//         assert(get<0>(test_match_three_server_one)[1] == "printer");
//         assert(get<0>(test_match_three_server_one).size() == 2);

//         // assert(get<0>(test_match_four).size() == 0);
//     }
            
//         // вводные: есть дублируемые в тексте документа слова
//         // id задан верно
//         // поисковый запрос не содержит слов содержащиеся в тексте документа
//         // минус слов нет
//         // документов дается несколько
//         // все документы актуальны
//     {
//         SearchServer server_two("from"s);
//         server_two.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//         server_two.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//         server_two.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_one);
//         server_two.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//         tuple<vector<string_view>, DocumentStatus> test_match_one_server_two = server_two.MatchDocument(search_query, id_one);
//         // tuple<vector<string_view>, DocumentStatus> test_match_two_server_two = server_two.MatchDocument("press"s, id_two);
//         tuple<vector<string_view>, DocumentStatus> test_match_three_server_two = server_two.MatchDocument(search_query, id_three);
//         tuple<vector<string_view>, DocumentStatus> test_match_four_server_two = server_two.MatchDocument(search_query, id_four);

//         assert(get<0>(test_match_one_server_two)[0] == "cat"s);
//         assert(get<0>(test_match_one_server_two).size() == 1);

//         // assert(get<0>(test_match_two_server_two).size() == 0);

//         assert(get<0>(test_match_three_server_two).size() == 2);
//         assert(get<0>(test_match_three_server_two)[0] == "cat"s);
//         assert(get<0>(test_match_three_server_two)[1] == "printer"s);

//         assert(get<0>(test_match_four_server_two).size() == 1);
//         assert(get<0>(test_match_four_server_two).size() == 1);
//     }


//     // вводные: есть дублируемые в тексте документа слова
//     // id задан верно
//     // поисковый запрос содержит слова содержащиеся в тексте документа
//     // минус слова есть
//     // документов дается несколько
//     // все документы актуальны
//     // {
//     // SearchServer server("from"s);
//     // server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//     // server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//     // tuple<vector<string_view>, DocumentStatus> test_match_one = server.MatchDocument(search_query, id_one);
//     // tuple<vector<string_view>, DocumentStatus> test_match_two = server.MatchDocument("-cat printer test"s, id_two);
//     // tuple<vector<string_view>, DocumentStatus> test_match_three = server.MatchDocument(search_query, id_three);
//     // tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument("cat printer test -pong"s, id_four);

//     // assert(get<0>(test_match_one)[0] == "cat"s);
//     // assert(get<0>(test_match_one).size() == 1);

//     // assert(get<0>(test_match_two).size() == 0);

//     // assert(get<0>(test_match_three)[0] == "cat"s);
//     // assert(get<0>(test_match_three)[1] == "printer"s);
//     // assert(get<0>(test_match_three).size() == 2);

//     // assert(get<0>(test_match_four)[0] == "test"s);
//     // assert(get<0>(test_match_four).size() == 1);
//     // }
//     // вводные: есть дублируемые в тексте документа слова
//     // id задан верно
//     // поисковый запрос содержит слова содержащиеся в тексте документа
//     // минус слова все
//     // документов дается несколько
//     // все документы актуальны
//     // {
//     // SearchServer server("from"s);
//     // server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//     // server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//     // tuple<vector<string_view>, DocumentStatus> test_match_one = server.MatchDocument("-cat -printer -test"s, id_one);
//     // tuple<vector<string_view>, DocumentStatus> test_match_two = server.MatchDocument(search_query, id_two);
//     // tuple<vector<string_view>, DocumentStatus> test_match_three = server.MatchDocument(search_query, id_three);
//     // tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument("cat printer test -pong -around -car"s, id_four);

//     // assert(get<0>(test_match_one).size() == 0);

//     // assert(get<0>(test_match_two).size() == 0);

//     // assert(get<0>(test_match_three)[0] == "cat"s);
//     // assert(get<0>(test_match_three)[1] == "printer"s);
//     // assert(get<0>(test_match_three).size() == 2);

//     // assert(get<0>(test_match_four)[0] == "test"s);
//     // assert(get<0>(test_match_four).size() == 1);
//     // }
//     // вводные: пустой документ
//     // id задан верно
//     // поисковый запрос есть
//     // минус слов нет
//     // документов дается несколько
//     // все документы актуальны
//     // {
//     // SearchServer server("from"s);
//     // server.AddDocument(id_one,      {},        DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//     // server.AddDocument(id_three,    {},      DocumentStatus::ACTUAL, rating_one);
//     // server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

//     // tuple<vector<string_view>, DocumentStatus> test_match_one = server.MatchDocument(search_query, id_one);
//     // tuple<vector<string_view>, DocumentStatus> test_match_two = server.MatchDocument(search_query, id_two);
//     // tuple<vector<string_view>, DocumentStatus> test_match_three = server.MatchDocument(search_query, id_three);
//     // tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument(search_query, id_four);

//     // assert(get<0>(test_match_one).size() == 0);

//     // assert(get<0>(test_match_two).size() == 0);

//     // assert(get<0>(test_match_three).size() == 0);

//     // assert(get<0>(test_match_four)[0] == "test"s);
//     // assert(get<0>(test_match_four).size() == 1);
//     // }

//     // вводные: есть дублируемые в тексте документа слова
//     // id задан верно
//     // поисковый запрос содержит слова содержащиеся в тексте документа
//     // минус слов нет
//     // документов дается несколько
//     // все документы разного статуса
//     // документу присвоен корректный статус
//     // {
//     // SearchServer server(""s);
//     // server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL,     rating_one);
//     // server.AddDocument(id_two,      doc_two,        DocumentStatus::BANNED,     rating_two);
//     // server.AddDocument(id_three,    doc_three,      DocumentStatus::IRRELEVANT, rating_one);
//     // server.AddDocument(id_four,     doc_four,       DocumentStatus::REMOVED,    rating_four);

//     // tuple<vector<string_view>, DocumentStatus> test_match_one = server.MatchDocument(search_query, id_one);
//     // tuple<vector<string_view>, DocumentStatus> test_match_two = server.MatchDocument(search_query, id_two);
//     // tuple<vector<string_view>, DocumentStatus> test_match_three = server.MatchDocument(search_query, id_three);
//     // tuple<vector<string_view>, DocumentStatus> test_match_four = server.MatchDocument(search_query, id_four);

//     // assert(get<0>(test_match_one)[0] == "cat"s);
//     // assert(get<0>(test_match_one).size() == 1);

//     // assert(get<0>(test_match_two).size() == 0);

//     // assert(get<0>(test_match_three)[0] == "cat"s);
//     // assert(get<0>(test_match_three)[1] == "printer"s);
//     // assert(get<0>(test_match_three).size() == 2);

//     // assert(get<0>(test_match_four)[0] == "test"s);
//     // assert(get<0>(test_match_four).size() == 1);

//     // assert(get<1>(test_match_one) == DocumentStatus::ACTUAL);
//     // assert(get<1>(test_match_two) == DocumentStatus::BANNED);
//     // assert(get<1>(test_match_three) == DocumentStatus::IRRELEVANT);
//     // assert(get<1>(test_match_four) == DocumentStatus::REMOVED);
//     // }
//     // cout << "Test TestFindTopDocuments: OK"s << endl;
// }

// // // Цель теста: проверить корректность возвращаемых id, relevance и rating
// // // проверить сортировку по релевантности 
// // // проверить фильтр по предикату (статус, выражение)
// // void TestFine() {
// //     int id_one = 1;
// //     int id_two = 2;
// //     int id_three = 3;
// //     int id_four = 4;
// //     string doc_one = "small cat in the small city"s;
// //     string doc_two = "dog in the town"s;
// //     string doc_three = "cat printer table in for car seven apple"s;
// //     string doc_four = "test school never again song in with"s;
// //     vector<int> rating_one = {-10, 0, 8};
// //     vector<int> rating_two = {-10, -6, -1};
// //     vector<int> rating_three = {1, 2, 12};
// //     vector<int> rating_four = {0};
// //     string search_query = "cat printer test"s;
// //     /*
// //     struct Document {
// //     int id;
// //     double relevance;
// //     int rating;
// // };
// //     */
// //     // вводные: 4 разных набора рейтинга
// //     // последовательные id
// //     // часть документов содержит поисковый запрос
// //     // минус слов нет
// //     // все документы имеют одинаковый статус
// //     // стоп слова не заданы
// //     {
// //     SearchServer server("");
// //     server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
// //     server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
// //     server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_three);
// //     server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);

// //     vector<Document> test_find_one = server.FindTopDocuments(search_query);
// //     vector<Document> test_find_two = server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0; });
// //     vector<Document> test_find_three = server.FindTopDocuments(search_query, DocumentStatus::BANNED);
// //     vector<Document> test_find_four = server.FindTopDocuments(search_query, DocumentStatus::ACTUAL);

// //     assert(test_find_one[0].id == 3);
// //     assert(test_find_one[0].rating == 5);
// //     assert(test_find_one[0].relevance < 0.25994 && test_find_one[0].relevance > 0.25992);
// //     assert(test_find_one[1].id == 4);
// //     assert(test_find_one[1].rating == 0);
// //     assert(test_find_one[1].relevance < 0.198043 && test_find_one[1].relevance > 0.198041);
// //     assert(test_find_one[2].id == 1);
// //     assert(test_find_one[2].rating == 0);
// //     assert(test_find_one[2].relevance < 0.115526 && test_find_one[2].relevance > 0.115524);

// //     assert(test_find_two[0].id == 4);
// //     assert(test_find_two[0].rating == 0);
// //     assert(test_find_two[0].relevance < 0.198043 && test_find_one[0].relevance > 0.198041);

// //     assert(test_find_three.size() == 0);

// //     }
// // }
// // Функция TestSearchServer является точкой входа для запуска тестов
// void TestSearchServer() {
//     TestExcludeStopWordsFromAddedDocumentContent();
//     TestMatch();
//     // TestFine();
// }

// // --------- Окончание модульных тестов поисковой системы -----------

// // int main() {
// //     TestSearchServer();
//     // SearchServer search_server("");
//     // int id_one = 1;
//     // int id_two = 2;
//     // int id_three = 3;
//     // int id_four = 4;
//     // string doc_one = "small cat in the small city"s;
//     // string doc_two = "dog in the town"s;
//     // string doc_three = "cat printer table in for car seven apple"s;
//     // string doc_four = "test school never again song in with"s;
//     // vector<int> rating_one = {-10, 0, 8};
//     // vector<int> rating_two = {-10, -6, -1};
//     // vector<int> rating_three = {1, 2, 12};
//     // vector<int> rating_four = {0};
//     // string search_query = "cat printer test"s;
//     // // search_server.SetStopWords("и в на"s);

//     // search_server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
//     // search_server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
//     // search_server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_three);
//     // search_server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);
    
//     // cout << "ACTUAL by default:"s << endl;
//     // for (const Document& document : search_server.FindTopDocuments(search_query)) {
//     //     PrintDocument(document);
//     // }
    
//     // cout << "BANNED:"s << endl;
//     // for (const Document& document : search_server.FindTopDocuments(search_query, DocumentStatus::BANNED)) {
//     //     PrintDocument(document);
//     // }
    
//     // cout << "Even ids:"s << endl;
//     // for (const Document& document : search_server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0; })) {
//     //     PrintDocument(document);
//     // }
// // } 


// int main() {
//     TestSearchServer();
// //     SearchServer search_server;
// //     search_server.("и в на"s);

// //     search_server.AddDocument(0, "белый кот и модный ошейник тест"s,   DocumentStatus::ACTUAL, {8, -3});
// //     search_server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
// //     search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
// //     search_server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});
    
// //     cout << "ACTUAL by default:"s << endl;
// //     for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s)) {
// //         PrintDocument(document);
// //     }
    
// //     cout << "BANNED:"s << endl;
// //     for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED)) {
// //         PrintDocument(document);
// //     }
    
// //     cout << "Even ids:"s << endl;
// //     for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0; })) {
// //         PrintDocument(document);
// //     }
    
// //     return 0;
// } 

// /*
// ответ:
// { document_id = 1, relevance = 0.866434, rating = 5 }
// { document_id = 0, relevance = 0.173287, rating = 2 }
// { document_id = 2, relevance = 0.173287, rating = -1 }
// BANNED:
// { document_id = 3, relevance = 0.231049, rating = 9 }
// Even ids:
// { document_id = 0, relevance = 0.173287, rating = 2 }
// { document_id = 2, relevance = 0.173287, rating = -1 }
// */

