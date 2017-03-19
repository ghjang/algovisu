#include "catch.hpp"

#include <type_traits>
#include <tuple>
#include <string>
#include <vector>
#include <iostream>

#include <boost/spirit/include/lex_lexertl.hpp>


namespace
{
    template <typename>
    struct TD;

    namespace lex = boost::spirit::lex;

    // op_token doesn't support spaces.
    template <typename Lexer>
    struct op_token : lex::lexer<Lexer>
    {
        op_token()
        {
            this->self = lex::char_('+')
                            | '-'
                            | '*'
                            | '/'
                            | '('
                            | ')';
        }
    };
}   // un-named namespace

TEST_CASE("Spirit.Lex getting default token id", "[lexer]")
{
    char const * pBegin = "+-*/()"; // no spaces in the source string.
    char const * pEnd = pBegin + std::strlen(pBegin);

    using lexer_t = lex::lexertl::lexer<>;
    using token_store_t = std::vector<std::pair<int, std::string>>;

    op_token<lexer_t> tokens;
    token_store_t tokenStore;

    REQUIRE(
        lex::tokenize(
                pBegin, pEnd,
                tokens,
                [&tokenStore](auto const& token) {
                    static_assert(
                        std::is_same<
                                lex::lexertl::token<>,
                                std::decay_t<decltype(token)>
                        >()
                    );

                    static_assert(
                        std::is_same<
                                boost::iterator_range<char const *>,
                                std::decay_t<decltype(token.value())>
                        >()
                    );
                    
                    tokenStore.emplace_back(
                        token.id(),
                        std::string{ std::begin(token.value()), std::end(token.value()) }
                    );
                    
                    return true;
                }
        )
    );

    REQUIRE(tokenStore.size() == 6);

    REQUIRE((std::get<0>(tokenStore[0]) == '+'));   // here, the default id for a char
                                                    // is the corresponding ASCII code.

    REQUIRE((std::get<1>(tokenStore[0]) == "+"));   // the token string

    REQUIRE((std::get<0>(tokenStore[1]) == '-'));
    REQUIRE((std::get<1>(tokenStore[1]) == "-"));
    REQUIRE((std::get<0>(tokenStore[2]) == '*'));
    REQUIRE((std::get<1>(tokenStore[2]) == "*"));
    REQUIRE((std::get<0>(tokenStore[3]) == '/'));
    REQUIRE((std::get<1>(tokenStore[3]) == "/"));
    REQUIRE((std::get<0>(tokenStore[4]) == '('));
    REQUIRE((std::get<1>(tokenStore[4]) == "("));
    REQUIRE((std::get<0>(tokenStore[5]) == ')'));
    REQUIRE((std::get<1>(tokenStore[5]) == ")"));
}
