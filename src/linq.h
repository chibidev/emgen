//
//  linq.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include <utility>

namespace linq {
    namespace detail {
        template<typename Iterator>
        class query_base {
        public:
            decltype(auto) begin() {
                return _begin;
            }

            decltype(auto) begin() const {
                return _begin;
            }

            decltype(auto) cbegin() const {
                return _begin;
            }

            decltype(auto) end() {
                return _end;
            }

            decltype(auto) end() const {
                return _end;
            }

            decltype(auto) cend() const {
                return _end;
            }

        protected:
            template<typename RangeIterator>
            query_base(RangeIterator&& begin, RangeIterator&& end)
            : _begin(std::forward<RangeIterator>(begin)),
            _end(std::forward<RangeIterator>(end))
            {
            }

        private:
            Iterator _begin;
            Iterator _end;
        };

        template<typename Iterator, typename Predicate>
        class filter_iterator {
        public:
            template<typename RangeIterator, typename FilterPredicate>
            filter_iterator(RangeIterator&& begin, RangeIterator&& end, RangeIterator&& current, FilterPredicate&& predicate)
            : _begin(std::forward<RangeIterator>(begin)),
            _end(std::forward<RangeIterator>(end)),
            _current(std::forward<RangeIterator>(current)),
            _predicate(std::forward<FilterPredicate>(predicate))
            {
                advance_while_first_valid();
            }

            decltype(auto) operator*() {
                return *_current;
            }

            decltype(auto) operator*() const {
                return *_current;
            }

            decltype(auto) operator->() {
                return _current.operator->();
            }

            decltype(auto) operator->() const {
                return _current.operator->();
            }

            filter_iterator& operator++() {
                ++_current;
                advance_while_first_valid();
                return *this;
            }

            bool operator!=(const filter_iterator& other) {
                return _current != other._current;
            }

        private:
            void advance_while_first_valid() {
                while (_current != _end && !_predicate(*_current))
                    ++_current;
            }

            Iterator _begin;
            Iterator _end;
            Iterator _current;
            Predicate _predicate;
        };

        template<typename Iterator, typename Transformer>
        class transform_query;

        template<typename Iterator, typename Predicate>
        class filtered_query : public query_base<Iterator> {
        public:
            template<typename RangeIterator, typename NextPredicate>
            filtered_query(RangeIterator&& begin, RangeIterator&& end, NextPredicate&& predicate)
            : query_base<Iterator>(std::forward<RangeIterator>(begin), std::forward<RangeIterator>(end)),
            _predicate(std::forward<NextPredicate>(predicate))
            {
            }

            template<typename NextPredicate>
            decltype(auto) where(NextPredicate&& predicate) {
                auto thisPred = _predicate;
                auto pred = [predicate, thisPred](auto&& item) {
                    return thisPred(item) && predicate(item);
                };
                return filtered_query<Iterator, decltype(pred)>{query_base<Iterator>::begin(), query_base<Iterator>::end(), pred};
            }

            template<typename Transformer>
            decltype(auto) transform(Transformer&& transformer) {
                return transform_query<decltype(this->begin()), Transformer>{ this->begin(), this->end(), std::forward<Transformer>(transformer)};
            }

            decltype(auto) begin() const {
                return filter_iterator<Iterator, Predicate>{ query_base<Iterator>::begin(), query_base<Iterator>::end(), query_base<Iterator>::begin(), _predicate};
            }

            decltype(auto) end() const {
                return filter_iterator<Iterator, Predicate>{ query_base<Iterator>::begin(), query_base<Iterator>::end(), query_base<Iterator>::end(), _predicate};
            }

        private:
            Predicate _predicate;
        };

        template<typename Iterator, typename Transformer>
        class transform_iterator {
        public:
            template<typename RangeIterator, typename Callable>
            transform_iterator(RangeIterator&& current, Callable&& transformer)
            : _current(std::forward<RangeIterator>(current)),
            _transformer(std::forward<Callable>(transformer))
            {
            }

            transform_iterator& operator++() {
                ++_current;
                return *this;
            }

            decltype(auto) operator*() {
                return _transformer(*_current);
            }

            decltype(auto) operator*() const {
                return _transformer(*_current);
            }

            bool operator!=(const transform_iterator& other) {
                return _current != other._current;
            }

        private:
            Iterator _current;
            Transformer _transformer;
        };

        template<typename Iterator, typename Transformer>
        class transform_query : query_base<Iterator> {
        public:
            template<typename RangeIterator, typename Callable>
            transform_query(RangeIterator&& begin, RangeIterator&& end, Callable&& transformer)
            : query_base<Iterator>(std::forward<RangeIterator>(begin), std::forward<RangeIterator>(end)),
            _transformer(std::forward<Callable>(transformer))
            {
            }

            decltype(auto) begin() const {
                return transform_iterator<Iterator, Transformer>{ query_base<Iterator>::begin(), _transformer };
            }

            decltype(auto) end() const {
                return transform_iterator<Iterator, Transformer>{ query_base<Iterator>::end(), _transformer };
            }

            template<typename Predicate>
            decltype(auto) where(Predicate&& predicate) {
                return filtered_query<decltype(this->begin()), Predicate>{this->begin(), this->end(), std::forward<Predicate>(predicate)};
            }

            template<typename Callable>
            decltype(auto) transform(Callable&& transformer) {
                auto thisTransf = _transformer;
                auto combined = [thisTransf, transformer](auto&& item) {
                    return transformer(thisTransf(item));
                };
                return transform_query<Iterator, decltype(combined)>{ query_base<Iterator>::begin(), query_base<Iterator>::end(), combined };
            }

        private:
            Transformer _transformer;
        };

        template<typename Iterator>
        class query : public query_base<Iterator> {
        public:
            template<typename RangeIterator>
            query(RangeIterator&& begin, RangeIterator&& end)
            : query_base<Iterator>(std::forward<RangeIterator>(begin), std::forward<RangeIterator>(end))
            {
            }

            template<typename Predicate>
            decltype(auto) where(Predicate&& predicate) {
                return filtered_query<decltype(this->begin()), Predicate>{ this->begin(), this->end(), std::forward<Predicate>(predicate) };
            }

            template<typename Transformer>
            decltype(auto) transform(Transformer&& transform) {
                return transform_query<decltype(this->begin()), Transformer>{ this->begin(), this->end(), std::forward<Transformer>(transform)} ;
            }
        };
    }

    template<typename Range>
    decltype(auto) query(Range&& range) {
        using std::begin;
        using std::end;
        return detail::query<decltype(begin(range))>{ begin(range), end(range) };
    }
}
