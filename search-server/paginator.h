// Copyright 2022
// 23:06 11/04/2022

#pragma once

#include <vector>
#include <cassert>

template <typename Iterator>
class IteratorRange {
 public:
    IteratorRange() = default;
    IteratorRange(Iterator conteiner_begin, Iterator conteiner_end) {
        begin_ = conteiner_begin;
        end_ = conteiner_end;
    }

    void FormPage(Iterator conteiner_begin, Iterator conteiner_end) {
        begin_ = conteiner_begin;
        end_ = conteiner_end;
    }

    auto begin()  {
        return begin_;
    }

    auto end() {
        return end_;
    }

 private:
    Iterator begin_;
    Iterator end_;
};

template <typename Iterator>
class Paginator {
 public:
    Paginator(Iterator container_begin, Iterator container_end, size_t page_size) {
        int interval = distance(container_begin, container_end);
        int size = static_cast<int>(page_size);
        auto vsp = container_begin;
        assert(container_end >= container_end && page_size > 0);
        advance(vsp, size);
        for (int i = 0; i != interval / size; i++) {
            IteratorRange<Iterator> buffer(container_begin, vsp);
            search_page_.push_back(buffer);
            advance(container_begin, size);
            advance(vsp, size);
        }
        if (interval % size != 0) {
            IteratorRange<Iterator> buffer(container_begin, container_end);
            search_page_.push_back(buffer);
        }
}

    auto begin() const {
        return search_page_.begin();
    }

    auto end() const {
        return search_page_.end();
    }

 private:
    std::vector<IteratorRange<Iterator>> search_page_;
};

template <typename Iterator>
std::ostream& std::operator<<(ostream& output, IteratorRange<Iterator> range) {
    for (auto range_i = range.begin(); range_i != range.end(); ++range_i) {
        output << "{ "s << "document_id = "s << range_i -> id;
        output << ", relevance = "s << range_i -> relevance;
        output << ", rating = " << range_i -> rating << " }"s;
    }
    return output;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
