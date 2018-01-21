//
//  generators.h
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 19..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma once

#include "linq.h"
#include "../cppast/extensions.h"
#include "../utilities/string.h"
#include "../utilities/functional.h"

#include "helpers.h"

namespace emgen {
    template<typename CppRange>
    decltype(auto) generate_enums(CppRange&& range, const std::string& prefix = {}) {
        auto&& enums = cppast::enums(range);
        auto&& exported_enums = emgen::exported(enums);

        ranges::for_each(exported_enums, [&](auto&& en) {
            auto&& enum_name = en.name();
            std::cout << "  enum_<" << prefix << enum_name << ">(\"" << enum_name << "\")" << std::endl;

            ranges::for_each(cppast::enum_values(en), [&](auto&& value) {
                auto&& value_name = value.name();
                std::cout << "    .value(\"" << value_name << "\", " << prefix << enum_name << "::" << value_name << ")" << std::endl;
            });

            std::cout << "  ;" << std::endl;
        });
    }

    template<typename CppRange>
    decltype(auto) generate_classes(CppRange&& range, const std::string& prefix = {}) {
        auto&& classes = cppast::classes(range);
        auto&& exported_classes = emgen::exported(classes);

        ranges::for_each(exported_classes, [&](auto&& cl) {
            auto&& class_name = cl.name();

            generate_enums(cl, prefix + class_name + "::");
            generate_classes(cl, prefix + class_name + "::");

            auto&& bases = emgen::typenames(linq::query(cl.bases()));

            auto&& public_methods = cppast::methods(cl).where([](auto&& method) {
                // TODO
                return true;
            });

            bool is_polymorphic = cppast::is_class_polymorphic(cl);
            auto&& wrapper_name = class_name + "_wrapper";

            if (is_polymorphic) {
                std::cout << "  struct " << wrapper_name << " : public em::wrapper<" << prefix << class_name << "> {" << std::endl;
                std::cout << "      EMSCRIPTEN_WRAPPER(" << wrapper_name << ");" << std::endl;

                auto&& virtual_methods = public_methods.where(&cppast::is_pure_method);
                auto&& non_abstract_virtual_methods = virtual_methods.where(utilities::functional::negate(&cppast::is_pure_method));

                ranges::for_each(non_abstract_virtual_methods, [](auto&& method) {
                    auto&& params = emgen::parameter_signatures(method.parameters());
                    auto&& param_names = emgen::names(method.parameters());

                    std::cout << "      " << cppast::to_string(method.return_type()) << " " << method.name() << "(" << utilities::string::join_with(params, ", ") << ") {" << std::endl;
                    std::cout << "          return call<" << cppast::to_string(method.return_type()) << ">(\"" << method.name() << "\"" << utilities::string::prefix_with(param_names, ", ") << ");" << std::endl;
                    std::cout << "      }" << std::endl;
                });

                auto&& pure_virtual_methods = virtual_methods.where(&cppast::is_pure_method);

                ranges::for_each(pure_virtual_methods, [](auto&& method) {
                    auto&& params = emgen::parameter_signatures(method.parameters());
                    auto&& param_names = emgen::names(method.parameters());

                    std::cout << "      " << cppast::to_string(method.return_type()) << " " << method.name() << "(" << utilities::string::join_with(params, ", ") << ") {" << std::endl;
                    std::cout << "          return call<" << cppast::to_string(method.return_type()) << ">(\"" << method.name() << "\"" << utilities::string::prefix_with(param_names, ", ") << ");" << std::endl;
                    std::cout << "      }" << std::endl;
                });

                std::cout << "  };" << std::endl;
            }

            /* HEADER */{
                std::cout << "  class_<" << prefix << class_name << utilities::string::prefix_with(bases, ", ") << ">(\"" << class_name << "\")" << std::endl;
                if (is_polymorphic)
                    std::cout << "    .allow_subclass<" << wrapper_name << ">(\"" << wrapper_name << "\")" << std::endl;

                std::cout << "    .smart_ptr<std::shared_ptr<" << prefix << class_name << ">>()" << std::endl;
            }

            {
                auto&& public_constructors = cppast::constructors(cl).where([](auto&& method) {
                    // TODO
                    return true;
                });
                ranges::for_each(public_constructors, [&](auto&& constructor) {
                    auto&& param_types = emgen::typenames(linq::query(constructor.parameters()));
                    std::cout << "    .constructor(" << utilities::string::join_with(param_types, ", ") << ")" << std::endl;
                });
            }

            {
                ranges::for_each(public_methods, [&](auto&& method) {
                    auto&& method_name = method.name();
                    if (cppast::is_virtual(method.virtual_info())) {
                        if (cppast::is_pure(method.virtual_info())) {
                            std::cout << "    .function(\"" << method_name << "\", &" << prefix << class_name << "::" << method_name << ", em::pure_virtual())" << std::endl;
                        } else {
                            auto&& params = emgen::parameter_signatures(method.parameters());
                            auto&& param_names = emgen::names(method.parameters());

                            std::cout << "    .function(\"" << method_name << "\", optional_override([](" << prefix << class_name << "& self" << utilities::string::prefix_with(params, ", ") << ") {" << std::endl;
                            std::cout << "        return self." << prefix << class_name << "::" << method_name << "(" << utilities::string::join_with(param_names, ", ") << ");" << std::endl;
                            std::cout << "    }))" << std::endl;
                        }
                    } else {
                        std::cout << "    .function(\"" << method_name << "\", &" << prefix << class_name << "::" << method_name << ")" << std::endl;
                    }
                });
            }

            {
                auto&& public_members = cppast::members(cl).where([](auto&& member) {
                    // TODO
                    return true;
                });

                ranges::for_each(public_members, [&](auto&& member) {
                    auto&& member_name = member.name();
                    std::cout << "    .field(\"" << member_name << "\", &" << prefix << class_name << "::" << member_name << ")" << std::endl;
                });
            }

            {
                auto&& functions = cppast::functions(cl);
                ranges::for_each(functions, [&](auto&& function) {
                    auto&& function_name = function.name();
                    std::cout << "    .class_function(\"" << function_name << "\", &" << prefix << class_name << "::" << function_name << ")" << std::endl;
                });
            }

            /* FOOTER */{
                std::cout << "  ;" << std::endl;
            }
        });
    }
}
