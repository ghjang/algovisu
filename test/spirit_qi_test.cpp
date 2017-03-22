#include "catch.hpp"

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/qi.hpp>


namespace
{
    namespace qi = boost::spirit::qi;
    namespace lex = boost::spirit::lex;

    // At least the iterator type should be provided for qi::parse.
    struct my_grammar : qi::grammar<char const *>
    {
        my_grammar()
            // qi::grammar's default constructor is deleted by design.
            // You need to call the constructor accepting a start rule explicitly.
            : my_grammar::base_type(start_)
        { }

        // The start rule should be compatible with the grammar.
        qi::rule<char const *> start_;
    };

    // At least the iterator and skipper types should be provided for qi::phrase_parse.
    struct my_phrase_grammar : qi::grammar<char const *, qi::ascii::space_type>
    {
        my_phrase_grammar()
            // qi::grammar's default constructor is deleted by design.
            // You need to call the constructor accepting a start rule explicitly.
            : my_phrase_grammar::base_type(start_)
        { }

        // The start rule should be compatible with the grammar.
        qi::rule<char const *, qi::ascii::space_type> start_;
    };

    // lex::lexertl::lexer is an implementation of the lex::lexer.
    struct my_lexer : lex::lexer<lex::lexertl::lexer<>>
    {
        my_lexer()
        {
            token_def_ = "some token def";
            this->self = token_def_;
        }

        lex::token_def<> token_def_;
    };

    // The iterator type should be specified as the lexer's iterator type.
    struct my_grammar_with_lexer : qi::grammar<my_lexer::iterator_type>
    {
        my_grammar_with_lexer(my_lexer const& l)
            : my_grammar_with_lexer::base_type(start_)
        {
            // This is OK because the iterator types are compatible.
            start_ = l.token_def_;
        }

        // The start rule should be compatible with the grammar.
        qi::rule<my_lexer::iterator_type> start_;
    };

    // lex::lexertl::lexer is an implementation of the lex::lexer.
    struct my_lexer_with_state : lex::lexer<lex::lexertl::lexer<>>
    {
        my_lexer_with_state()
        {
            // this is for the INITIAL state.
            token_def_ = "some token def";
            this->self = token_def_;

            // this is for the WS custom state.
            white_space_ = "\\s";
            this->self("WS") = white_space_;
        }

        lex::token_def<> token_def_;
        lex::token_def<> white_space_;
    };

    // The iterator type should be specified as the lexer's iterator type.
    struct my_phrase_grammar_with_lexer
            : qi::grammar<
                    my_lexer_with_state::iterator_type,
                    qi::in_state_skipper<my_lexer_with_state::lexer_def>
              >
    {
        my_phrase_grammar_with_lexer(my_lexer_with_state const& l)
            : my_phrase_grammar_with_lexer::base_type(start_)
        {
            // This is OK because the iterator types are compatible.
            start_ = l.token_def_;
        }

        // The start rule should be compatible with the grammar.
        qi::rule<
                my_lexer_with_state::iterator_type,
                qi::in_state_skipper<my_lexer_with_state::lexer_def>
        > start_;
    };
}   // un-named namespace


TEST_CASE("qi::parse function compile test", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_grammar g;
    qi::parse(pBegin, pEnd, g);

    // This is compiled!
    // You can pass a non-phrased grammar to qi::phrase_parse.
    qi::phrase_parse(pBegin, pEnd, g, qi::ascii::space);
}

TEST_CASE("qi::phrase_parse function compile test", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_phrase_grammar g;
    qi::phrase_parse(pBegin, pEnd, g, qi::ascii::space);

    // This isn't compiled.
    // You can't pass a phrased grammar to qi::parse.
    //qi::parse(pBegin, pEnd, g);
}

TEST_CASE("qi::parse function compile test with lexer", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_lexer l;
    my_grammar_with_lexer g{ l };
    
    // The type of begin and end is same as my_lexer's iterator type.
    auto begin = l.begin(pBegin, pEnd);
    auto end = l.end();

    qi::parse(begin, end, g);

    // This isn't compiled. qi::ascii::space is not compatible with the grammar.
    //qi::phrase_parse(begin, end, g, qi::ascii::space);

    // This is compiled.
    //
    // Note that this is not a working example. It's just for compile test.
    // In real programs, you should specifiy the actual state name
    // in the qi::in_state's first argument.
    //
    // qi::in_state will return l.self between the square brackets,
    // which is a compatible skipper.
    qi::phrase_parse(begin, end, g, qi::in_state("")[l.self]);
}

TEST_CASE("qi::phrase_parse function compile test with lexer", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_lexer_with_state l;
    my_phrase_grammar_with_lexer g{ l };

    // The type of begin and end is same as my_lexer_with_state's iterator type.
    auto begin = l.begin(pBegin, pEnd);
    auto end = l.end();

    // WS is a state of my_lexer_with_state.
    qi::phrase_parse(begin, end, g, qi::in_state("WS")[l.self]);

    // This isn't compield. You can't pass a phased grammar to qi::parse.
    //qi::parse(begin, end, g);
}
