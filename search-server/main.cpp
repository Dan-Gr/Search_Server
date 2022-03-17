// -------- Начало модульных тестов поисковой системы ----------

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}

void TestFind() {
    int id_one = 1;
    int id_two = 2;
    int id_three = 3;
    int id_four = 4;
    string doc_one = "small cat in the small city"s;
    string doc_two = "dog in the town"s;
    string doc_three = "cat printer table in for car seven apple"s;
    string doc_four = "test school never again song in with"s;
    vector<int> rating_one = {-10, 0, 8};
    vector<int> rating_two = {-10, -6, -1};
    vector<int> rating_three = {1, 4, 12};
    vector<int> rating_four = {0};
    string search_query = "cat printer test"s;

    {
    SearchServer server;
    server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("small"s).size()), 1, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("small"s)[0].id), 1, "FindTopDocuments: FAIL"s);
    server.SetStopWords("small"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("small"s).size()), 0, "FindTopDocuments: FAIL"s);
    }
    // последовательные id
    // часть документов содержит поисковый запрос
    // минус слов нет
    // все документы имеют одинаковый статус
    // стоп слова не заданы
    {
    SearchServer server;
    server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
    server.AddDocument(id_two,      doc_two,        DocumentStatus::ACTUAL, rating_two);
    server.AddDocument(id_three,    doc_three,      DocumentStatus::ACTUAL, rating_three);
    server.AddDocument(id_four,     doc_four,       DocumentStatus::ACTUAL, rating_four);
    // проверка верного присвоения id, rating
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[0].id), 3, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[0].rating), 5, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<bool>(server.FindTopDocuments(search_query)[0].relevance < 0.25994), true, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<bool>(server.FindTopDocuments(search_query)[0].relevance > 0.25992), true, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[1].id), 4, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[1].rating), 0, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[1].relevance < 0.198043), true, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[1].relevance > 0.198041), true, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[2].id), 1, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[2].rating), 0, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[2].relevance < 0.115526), true, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query)[2].relevance > 0.115524), true, "FindTopDocuments: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) \
    { return document_id % 2 == 0; })[0].id), 4, "FindTopDocuments: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) \
    { return document_id % 2 == 0; })[0].rating), 0, "FindTopDocuments: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) \
    { return document_id % 2 == 0; })[0].relevance < 0.198043), true, "FindTopDocuments: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) \
    { return document_id % 2 == 0; })[0].relevance > 0.198041), true, "FindTopDocuments: FAIL"s);
    }

}

void TestMatchDocument() {
    int id_one = 1;
    int id_two = 2;
    int id_three = 3;
    int id_four = 4;
    string doc_one = "small cat in the small city"s;
    string doc_two = "dog in the town"s;
    string doc_three = "cat printer table in for car seven apple"s;
    string doc_four = "test school never again song in with"s;
    vector<int> rating_one = {-10, 0, 8};
    vector<int> rating_two = {-10, -6, -1};
    vector<int> rating_three = {1, 4, 12};
    vector<int> rating_four = {0};
    string search_query = "cat printer test"s;
    // вводные: есть дублируемые в тексте документа слова
    // id задан верно
    // поисковый запрос содержит слова содержащиеся в тексте документа
    // минус слов нет
    // документов дается несколько
    {
    SearchServer server;
    server.AddDocument(id_one,      doc_one,        DocumentStatus::ACTUAL, rating_one);
    server.AddDocument(id_two,      doc_two,        DocumentStatus::BANNED, rating_two);
    server.AddDocument(id_three,    doc_three,      DocumentStatus::IRRELEVANT, rating_one);
    server.AddDocument(id_four,     doc_four,       DocumentStatus::REMOVED, rating_four);

    ASSERT_EQUAL_HINT(static_cast<string>(get<0>(server.MatchDocument(search_query, id_one))[0]), "cat", "MatchDocument: FAIL"s);
    
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, id_one)).size()), 1, "MatchDocument: FAIL"s);
    
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, id_two)).size()), 0, "MatchDocument: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<string>(get<0>(server.MatchDocument(search_query, id_three))[0]), "cat"s, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<string>(get<0>(server.MatchDocument(search_query, id_three))[1]), "printer"s, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, id_three)).size()), 2, "MatchDocument: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<string>(get<0>(server.MatchDocument(search_query, id_four))[0]), "test"s, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, id_four)).size()), 1, "MatchDocument: FAIL"s);

    // поисковый запрос не содержит слов содержащиеся в тексте документов
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument("press"s, id_two)).size()), 0, "MatchDocument: FAIL"s);

    // минус слова есть
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument("-cat printer test"s, id_two)).size()), 0, "MatchDocument: FAIL"s);
    // минус слова в документах нет
    ASSERT_EQUAL_HINT(static_cast<string>(get<0>(server.MatchDocument("cat printer test -pong"s, id_four))[0]), "test"s, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument("cat printer test -pong"s, id_four)).size()), 1, "MatchDocument: FAIL"s);
    // минус слова все
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument("-cat -printer -test"s, id_one)).size()), 0, "MatchDocument: FAIL"s);

    // добавляем пустой документ
    server.AddDocument(25,      {},        DocumentStatus::ACTUAL, {});

    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, 25)).size()), 0, "MatchDocument: FAIL"s);
    // проверка присвоения статусов
    ASSERT_EQUAL_HINT(static_cast<int>(get<1>(server.MatchDocument(search_query, id_one))), 0, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<1>(server.MatchDocument(search_query, id_two))), 2, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<1>(server.MatchDocument(search_query, id_three))), 1, "MatchDocument: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(get<1>(server.MatchDocument(search_query, id_four))), 3, "MatchDocument: FAIL"s);

    
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::BANNED).size()), 0, "FindTopDocuments: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::ACTUAL).size()), 1, "FindTopDocuments: FAIL"s);
    
    }
}


void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestFind);
    RUN_TEST(TestMatchDocument);
    // Не забудьте вызывать остальные тесты здесь
}


// --------- Окончание модульных тестов поисковой системы -----------
