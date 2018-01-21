//
//  helpers.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include <string>

#include "ranges.h"
#include "linq.h"

using namespace std::literals;

namespace emgen {
    const std::string export_attribute_name = "jsexport"s;

    template<typename CppEntity>
    bool is_exported(CppEntity&& entity) {
        return ranges::any_of(entity.attributes(), [](auto&& attribute) {
            return attribute.name() == export_attribute_name;
        });
    }

    template<typename CppRange>
    decltype(auto) exported(CppRange&& range) {
        return range.where([](auto&& enum_ent) {
            return emgen::is_exported(enum_ent);
        });
    }

    template<typename Variable>
    decltype(auto) variable_signature(Variable&& var) {
        return cppast::to_string(var.type()) + " " + var.name();
    }

    template<typename ParameterRange>
    decltype(auto) parameter_signatures(ParameterRange&& range) {
        return linq::query(range).transform([](auto&& param) {
            return variable_signature(param);
        });
    }

    template<typename CppRange>
    decltype(auto) names(CppRange&& range) {
        return linq::query(range).transform([](auto&& elem) {
            return elem.name();
        });
    }

    template<typename CppRange>
    decltype(auto) typenames(CppRange&& range) {
        return range.transform([](auto&& elem) {
            return cppast::to_string(elem.type());
        });
    }
}
