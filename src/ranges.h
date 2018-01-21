//
//  ranges.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include <algorithm>
#include <utility>
#include <numeric>

namespace ranges {
    template<typename Range, typename Callable>
    decltype(auto) for_each(Range&& range, Callable&& action) {
        using std::begin;
        using std::end;
        return std::for_each(begin(range), end(range), std::forward<Callable>(action));
    }

    template<typename Range, typename Callable>
    decltype(auto) any_of(Range&& range, Callable&& predicate) {
        using std::begin;
        using std::end;
        return std::any_of(begin(range), end(range), std::forward<Callable>(predicate));
    }

    template<typename Range, typename T, typename BinaryOp>
    decltype(auto) accumulate(Range&& range, T&& value, BinaryOp&& op) {
        using std::begin;
        using std::end;

        return std::accumulate(begin(range), end(range), std::forward<T>(value), std::forward<BinaryOp>(op));
    }

    template<typename Range, typename BinaryOp>
    decltype(auto) accumulate(Range&& range, BinaryOp&& op) {
        using std::begin;
        using std::end;

        auto&& b = begin(range);
        auto&& e = end(range);
        if (b != e)
            return std::accumulate(++begin(range), end(range), *(begin(range)), std::forward<BinaryOp>(op));

        return decltype(std::declval<BinaryOp>()(*b, *b)){};
    }
}
