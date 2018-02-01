//
//  extensions.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include <utility>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_member_function.hpp>

#include "linq.h"

namespace cppast {
    template<typename Range, typename AstType>
    decltype(auto) children_of_type(Range&& range) {
        return linq::query(std::forward<Range>(range))
        .where([](auto&& item) {
            return item.kind() == AstType::kind();
        }).transform([](auto&& item) -> const AstType& {
            return static_cast<const AstType&>(item);
        });
    }

    bool is_virtual_method(const cppast::cpp_member_function& function) {
        return cppast::is_virtual(function.virtual_info());
    }

    bool is_pure_method(const cppast::cpp_member_function& function) {
        return cppast::is_pure(function.virtual_info());
    }

    template<typename Range>
    decltype(auto) classes(Range&& range) {
        return children_of_type<Range, cppast::cpp_class>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) enums(Range&& range) {
        return children_of_type<Range, cppast::cpp_enum>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) enum_values(Range&& range) {
        return children_of_type<Range, cppast::cpp_enum_value>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) members(Range&& range) {
        return children_of_type<Range, cppast::cpp_member_variable>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) functions(Range&& range) {
        return children_of_type<Range, cppast::cpp_function>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) methods(Range&& range) {
        return children_of_type<Range, cppast::cpp_member_function>(std::forward<Range>(range));
    }

    template<typename Range>
    decltype(auto) constructors(Range&& range) {
        return children_of_type<Range, cppast::cpp_constructor>(std::forward<Range>(range));
    }

    bool is_class_polymorphic(const cppast::cpp_class& cl) {
        for (auto&& method : methods(cl)) {
            if (cppast::is_virtual_method(method))
                return true;
        }

        return false;
    }

    template<typename T>
    bool is_public(T&& t) {
        return t.access_specifier() == cppast::cpp_access_specifier_kind::cpp_public;
    }
}
