// -------- Начало модульных тестов поисковой системы ----------
// вспомогательная функция для тестов
SearchServer CreateTestServer(int one, int two, int three, int four) {
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
    vector<int> rating_three = {1, 4, 16};
    vector<int> rating_four = {0};

    SearchServer server;
    server.AddDocument(id_one,      doc_one,        static_cast<DocumentStatus>(one), rating_one);
    server.AddDocument(id_two,      doc_two,        static_cast<DocumentStatus>(two), rating_two);
    server.AddDocument(id_three,    doc_three,      static_cast<DocumentStatus>(three), rating_three);
    server.AddDocument(id_four,     doc_four,       static_cast<DocumentStatus>(four), rating_four);
    
    return server;
}
// проверка добавления документов
void TestAddDocuments() {
    string search_query = "cat printer test dog song"s;
    SearchServer server = CreateTestServer(0, 1, 2, 3);

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query)[0].id, 1, "TestAddDocumenst: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[0].id, 2, "Add Documents: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::BANNED)[0].id, 3, "Add Documents: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::REMOVED)[0].id, 4, "Add Documents: FAIL"s);
    
    vector<string> v_doc_one = {"small"s, "cat"s, "in"s, "the"s, "small"s, "city"s};
    vector<string> v_doc_two = {"dog"s, "in"s, "the"s, "town"s};
    vector<string> v_doc_three = {"cat"s, "printer"s, "in"s, "for"s, "car"s, "seven"s, "apple"s};
    vector<string> v_doc_four = {"test"s, "school"s, "never"s, "again"s, "song"s, "in"s, "with"s};

    server = CreateTestServer(0, 0, 0, 0);
    for (int i = 0; i < static_cast<int>(v_doc_one.size()-1); i++) {
        ASSERT_HINT(!server.FindTopDocuments(v_doc_one[i]).empty(), "Add Documents: FAIL"s);
        ASSERT_EQUAL_HINT(server.FindTopDocuments(v_doc_one[i])[0].id == 1 || 3, true, "Add Documents: FAIL"s);
    }

    for (int i = 0; i < static_cast<int>(v_doc_two.size()-1); i++) {
        ASSERT_HINT(!server.FindTopDocuments(v_doc_two[i]).empty(), "Add Documents: FAIL"s);
        ASSERT_EQUAL_HINT(server.FindTopDocuments(v_doc_two[i])[0].id == 3 || 2, true, "Add Documents: FAIL"s);
    }

    for (int i = 0; i < static_cast<int>(v_doc_three.size()-1); i++) {
        ASSERT_HINT(!server.FindTopDocuments(v_doc_three[i]).empty(), "Add Documents: FAIL"s);
        ASSERT_EQUAL_HINT(server.FindTopDocuments(v_doc_three[i])[0].id == 1 || 3, true, "Add Documents: FAIL"s);
    }

    for (int i = 0; i < static_cast<int>(v_doc_four.size()-1); i++) {
        ASSERT_HINT(!server.FindTopDocuments(v_doc_four[i]).empty(), "Add Documents: FAIL"s);
        ASSERT_EQUAL_HINT(server.FindTopDocuments(v_doc_four[i])[0].id == 4 || 3, true, "Add Documents: FAIL"s);
    }

    // поисковый запрос не содержит слов содержащиеся в тексте документов
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument("press"s, 2)).size()), 0, "Add Documents: FAIL"s);
    // добавляем пустой документ
    server.AddDocument(25,      {},        DocumentStatus::ACTUAL, {});
    ASSERT_EQUAL_HINT(static_cast<int>(get<0>(server.MatchDocument(search_query, 25)).size()), 0, "Add Documents: FAIL"s);

}


// проверка отсеивания стоп слов
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
// проверка порядка сортировки по релевантности
void TestFindCheckTrueRelevance() {
    SearchServer server = CreateTestServer(0, 0, 0, 0);
    string search_query = "cat printer test"s;
    
    ASSERT_EQUAL_HINT((abs(server.FindTopDocuments(search_query)[0].relevance) > abs(server.FindTopDocuments(search_query)[1].relevance)), true, "Sort Relevance: FAIL"s);
    ASSERT_EQUAL_HINT((abs(server.FindTopDocuments(search_query)[1].relevance) > abs(server.FindTopDocuments(search_query)[2].relevance)), true, "Sort Relevance: FAIL"s);
    ASSERT_EQUAL_HINT((abs(server.FindTopDocuments(search_query)[0].relevance) > abs(server.FindTopDocuments(search_query)[2].relevance)), true, "Sort Relevance: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query).size()), 3, "Sort Relevance: FAIL"s);

    server = CreateTestServer(0, 1, 1, 1);

    ASSERT_EQUAL_HINT((abs(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[0].relevance) > \
    abs(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[1].relevance)), true, "Sort Relevance: FAIL"s);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT).size()), 2, "Sort Relevance: FAIL"s);
}
// проверка расчета релевантности
void TestFindCheckRelevance() {
    string search_query = "cat printer test dog song"s;
    SearchServer server = CreateTestServer(0, 0, 0, 0);
    // в собственном коде выносил в глобальную переменную, здесь оставил так, т.к. иначе не проходит автотесты
    ASSERT_EQUAL_HINT(abs(server.FindTopDocuments(search_query)[0].relevance - 0.396084) < 1e-6, true, "True Relevance: FAIL"s);
    ASSERT_EQUAL_HINT(abs(server.FindTopDocuments(search_query)[1].relevance - 0.346574) < 1e-6, true, "True Relevance: FAIL"s);
    ASSERT_EQUAL_HINT(abs(server.FindTopDocuments(search_query)[2].relevance - 0.25993) < 1e-6, true, "True Relevance: FAIL"s);
    ASSERT_EQUAL_HINT(abs(server.FindTopDocuments(search_query)[3].relevance - 0.115525 < 1e-6), true, "True Relevance: FAIL"s);
}

// проверка работы функции-предиката
void TestFindCheckSearchPredicate() {
    SearchServer server = CreateTestServer(0, 0, 0, 0);
    string search_query = "cat printer test"s;

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 2 == 0;})[0].id, \
    4, "Search Predicate: FAIL"s);

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 1 == 0;})[0].id, \
    3, "Search Predicate: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 1 == 0;})[1].id, \
    4, "Search Predicate: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, [](int document_id, DocumentStatus, int) { return document_id % 1 == 0;})[2].id, \
    1, "Search Predicate: FAIL"s);

}
// проверка поиска по статусу
void TestFindCheckSearchStatus() {
    string search_query = "cat printer test"s;

    SearchServer server = CreateTestServer(0, 1, 1, 1);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[0].id, 3, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[1].id, 4, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT).size()), 2, "Search Status: FAIL");

    server = CreateTestServer(0, 1, 2, 3);
    search_query = "cat printer test dog song"s;

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::ACTUAL)[0].id, 1, "Search Status: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::ACTUAL).size()), 1, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT)[0].id, 2, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::IRRELEVANT).size()), 1, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::BANNED)[0].id, 3, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::BANNED).size()), 1, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::REMOVED)[0].id, 4, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::REMOVED).size()), 1, "Search Status: FAIL");

    server = CreateTestServer(2, 2, 2, 3);

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::BANNED)[0].id, 2, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::BANNED)[1].id, 3, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::BANNED)[2].id, 1, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::BANNED).size()), 3, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query, DocumentStatus::REMOVED)[0].id, 4, "Search Status: FAIL");
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments(search_query, DocumentStatus::REMOVED).size()), 1, "Search Status: FAIL");
}
// проверка расчета рейтинга
void TestFindCheckRating() {
    string search_query = "cat printer test dog song"s;
    SearchServer server = CreateTestServer(0, 0, 0, 0);

    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query)[0].rating, 0, "True Rating: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query)[1].rating, -5, "True Rating: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query)[2].rating, 7, "True Rating: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments(search_query)[3].rating, 0, "True Rating: FAIL"s);
}
// проверка на минус слова
void TestFindCheckMinesWords() {
    SearchServer server = CreateTestServer(0, 0, 0, 0);

    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("-cat printer test"s).size()), 1, "Mines Words: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments("-cat printer test"s)[0].id, 4, "Mines Words: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("cat printer test -pong"s).size()), 3, "Mines Words: FAIL"s);
    ASSERT_EQUAL_HINT(server.FindTopDocuments("-cat printer test"s)[0].id == 1 || 3 || 4, true, "Mines Words: FAIL"s);
    ASSERT_EQUAL_HINT(static_cast<int>(server.FindTopDocuments("-cat -printer -test"s).size()), 0, "Mines Words: FAIL"s);
}

void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestFindCheckTrueRelevance);
    RUN_TEST(TestFindCheckSearchPredicate);
    RUN_TEST(TestFindCheckSearchStatus);
    RUN_TEST(TestAddDocuments);
    RUN_TEST(TestFindCheckRating);
    RUN_TEST(TestFindCheckRelevance);
    RUN_TEST(TestFindCheckMinesWords);
}

// --------- Окончание модульных тестов поисковой системы -----------
