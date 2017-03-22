#include "catch.hpp"

#include <boost/spirit/include/qi.hpp>


namespace
{
    namespace qi = boost::spirit::qi;

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
}   // un-named namespace


TEST_CASE("qi::parse function compile test", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_grammar g;
    qi::parse(pBegin, pEnd, g);

    // This is compiled!
    qi::phrase_parse(pBegin, pEnd, g, qi::ascii::space);
}

TEST_CASE("qi::phrase_parse function compile test", "[parser]")
{
    char const * pBegin = "";
    char const * pEnd = pBegin + std::strlen(pBegin);

    my_phrase_grammar g;
    qi::phrase_parse(pBegin, pEnd, g, qi::ascii::space);

    // This isn't compiled.
    //qi::parse(pBegin, pEnd, g);
}
