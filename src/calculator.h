#ifndef ALGOVISU_CACULATOR_H
#define ALGOVISU_CACULATOR_H


#include <boost/spirit/include/lex_lexertl.hpp>


namespace algovisu
{
    namespace lex = boost::spirit::lex;

    template <typename Lexer>
    struct calc_token : lex::lexer<Lexer>
    {
        calc_token()
        {
            decimal_integer_ = R"(0|[1-9]\d*)";

            this->self = decimal_integer_
                            | '+' | '-'
                            | '*' | '/'
                            | '(' | ')';

            white_space_ = R"(\s+)";
            this->self("WS") = white_space_;
        }

        lex::token_def<> decimal_integer_;
        lex::token_def<> white_space_;
    };
} // namespace algovisu


#endif  // ALGOVISU_CACULATOR_H
