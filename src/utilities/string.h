//
//  string.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include <utility>
#include <string>
#include "ranges.h"

namespace utilities {
    namespace string {
        template<typename Range>
        decltype(auto) join_with(Range&& range, const std::string& str) {
            return ranges::accumulate(std::forward<Range>(range), [&](auto&& lhs, auto&& rhs) {
                return lhs + str + rhs;
            });
        }

        template<typename Range>
        decltype(auto) prefix_with(Range&& range, const std::string& str) {
            return ranges::accumulate(std::forward<Range>(range), std::string{}, [&](auto&& lhs, auto&& rhs) {
                return lhs + str + rhs;
            });
        }
    }
}
