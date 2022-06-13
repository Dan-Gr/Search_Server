// Copyright 2022
// 16:32 11/06/2022

#pragma once

#include <vector>
#include <string>
#include <execution>
#include <numeric>
#include <iostream>
#include <list>

#include "document.h"
#include "search_server.h"

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);

vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);
