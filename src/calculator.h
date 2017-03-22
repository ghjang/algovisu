#ifndef ALGOVISU_CACULATOR_H
#define ALGOVISU_CACULATOR_H


#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/qi.hpp>


namespace algovisu
{
    namespace lex = boost::spirit::lex;
    namespace qi = boost::spirit::qi;

    template <typename Lexer>
    struct calc_token : lex::lexer<Lexer>
    {
        calc_token()
        {
            // NOTE: leading zero is not allowed.
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

    template <typename Lexer>
    struct calc_grammar
            : qi::grammar<
                    typename Lexer::iterator_type,
                    qi::in_state_skipper<typename Lexer::lexer_def>
              >
    {
        template <typename TokenDef>
        calc_grammar(TokenDef const& tok)
            : calc_grammar::base_type(additive_expr_)
        {
            // NOTE: rules are defined from lowest priority to highest priority.

            using qi::char_;

            additive_expr_ = multiplicative_expr_
                                >> *(
                                        char_('+') >> multiplicative_expr_
                                    |   char_('-') >> multiplicative_expr_
                                    );

            multiplicative_expr_ = factor_
                                    >> *(
                                            char_('*') >> factor_
                                        |   char_('/') >> factor_
                                        );

            factor_ = tok.decimal_integer_
                            | '(' >> additive_expr_ >> ')';
        }

        using iterator_t = typename Lexer::iterator_type;
        using skipper_t = qi::in_state_skipper<typename Lexer::lexer_def>;

        qi::rule<iterator_t, skipper_t> additive_expr_;
        qi::rule<iterator_t, skipper_t> multiplicative_expr_;
        qi::rule<iterator_t, skipper_t> factor_;
    };
} // namespace algovisu


#endif  // ALGOVISU_CACULATOR_H
