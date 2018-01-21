//
//  functional.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

namespace utilities {
    namespace functional {
        template<typename Callable>
        decltype(auto) negate(Callable&& callable) {
            return [callable](auto&&... params) {
                return !callable(std::forward<decltype(params)>(params)...);
            };
        }
    }
}
