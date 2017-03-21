#include "catch.hpp"

#include "calculator.h"


namespace
{
    using token_store_t = std::vector<std::pair<int, std::string>>;

    // returns empty token store if the tokenization fails.
    template <typename Tokens>
    auto tokenize(char const * pBegin, char const * pEnd, Tokens const& tokens)
    {
        namespace lex = boost::spirit::lex;

        token_store_t tokenStore;
        bool result = lex::tokenize(
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
                        );
        if (!result) {
            tokenStore.clear();
        }
        return tokenStore;
    }
}   // un-named namespace


TEST_CASE("calc token", "[algovisu]")
{
    using namespace algovisu;
    
    //==========================================================================
    char const * pBegin = "+-*/()";
    char const * pEnd = pBegin + std::strlen(pBegin);

    using lexer_t = lex::lexertl::lexer<>;

    auto tokenStore = tokenize(pBegin, pEnd, calc_token<lexer_t>{});

    REQUIRE(!tokenStore.empty());

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

    //==========================================================================
    pBegin = "1234567890+-*/()";
    pEnd = pBegin + std::strlen(pBegin);
    tokenStore = tokenize(pBegin, pEnd, calc_token<lexer_t>{});

    REQUIRE(!tokenStore.empty());
    REQUIRE(tokenStore.size() == 7);

    REQUIRE((std::get<0>(tokenStore[0]) == 65536));
    REQUIRE((std::get<1>(tokenStore[0]) == "1234567890"));

    //==========================================================================
    pBegin = "0123456789+-*/()";
    pEnd = pBegin + std::strlen(pBegin);
    tokenStore = tokenize(pBegin, pEnd, calc_token<lexer_t>{});

    REQUIRE(!tokenStore.empty());
    REQUIRE(tokenStore.size() == 8);

    // NOTE: the leading zero for the decimal integer is allowed
    //          at the tokenization phase, but it will be an error
    //          at the syntax analysis phase.
    REQUIRE((std::get<0>(tokenStore[0]) == 65536));
    REQUIRE((std::get<1>(tokenStore[0]) == "0"));

    REQUIRE((std::get<0>(tokenStore[1]) == 65536));
    REQUIRE((std::get<1>(tokenStore[1]) == "123456789"));

    //==========================================================================
    pBegin = "10+(20-5)*7/2";
    pEnd = pBegin + std::strlen(pBegin);
    tokenStore = tokenize(pBegin, pEnd, calc_token<lexer_t>{});    

    REQUIRE(!tokenStore.empty());
    REQUIRE(tokenStore.size() == 11);

    REQUIRE((std::get<0>(tokenStore[0]) == 65536));
    REQUIRE((std::get<1>(tokenStore[0]) == "10"));
    REQUIRE((std::get<0>(tokenStore[1]) == '+'));
    REQUIRE((std::get<1>(tokenStore[1]) == "+"));
    REQUIRE((std::get<0>(tokenStore[2]) == '('));
    REQUIRE((std::get<1>(tokenStore[2]) == "("));
    REQUIRE((std::get<0>(tokenStore[3]) == 65536));
    REQUIRE((std::get<1>(tokenStore[3]) == "20"));
    REQUIRE((std::get<0>(tokenStore[4]) == '-'));
    REQUIRE((std::get<1>(tokenStore[4]) == "-"));
    REQUIRE((std::get<0>(tokenStore[5]) == 65536));
    REQUIRE((std::get<1>(tokenStore[5]) == "5"));
    REQUIRE((std::get<0>(tokenStore[6]) == ')'));
    REQUIRE((std::get<1>(tokenStore[6]) == ")"));
    REQUIRE((std::get<0>(tokenStore[7]) == '*'));
    REQUIRE((std::get<1>(tokenStore[7]) == "*"));
    REQUIRE((std::get<0>(tokenStore[8]) == 65536));
    REQUIRE((std::get<1>(tokenStore[8]) == "7"));
    REQUIRE((std::get<0>(tokenStore[9]) == '/'));
    REQUIRE((std::get<1>(tokenStore[9]) == "/"));
    REQUIRE((std::get<0>(tokenStore[10]) == 65536));
    REQUIRE((std::get<1>(tokenStore[10]) == "2"));
}
