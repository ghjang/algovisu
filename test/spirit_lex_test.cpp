#include "catch.hpp"

#include <type_traits>
#include <tuple>
#include <string>
#include <vector>
#include <iostream>

#include <boost/spirit/include/lex_lexertl.hpp>


//==============================================================================
namespace
{
    // for debugging
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

TEST_CASE("Spirit.Lex getting default token info", "[lexer]")
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

//==============================================================================
namespace
{
    // op_token doesn't support spaces.
    template <typename Lexer>
    struct op_token_with_white_space : lex::lexer<Lexer>
    {
        op_token_with_white_space()
        {
            white_space_ = "[ \\t\\n]+";

            this->self = lex::char_('+')
                            | '-'
                            | '*'
                            | '/'
                            | '('
                            | ')'
                            | white_space_;   
        }

        lex::token_def<> white_space_;
    };
}   // un-named namespace

TEST_CASE("Spirit.Lex getting default token info with white spaces", "[lexer]")
{
    // NOTE: this string has 6 white space tokens.
    char const * pBegin = R"(   +   -   * /
                                ()
                            )";
    char const * pEnd = pBegin + std::strlen(pBegin);

    using lexer_t = lex::lexertl::lexer<>;
    using token_store_t = std::vector<std::pair<int, std::string>>;

    op_token_with_white_space<lexer_t> tokens;
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

    /*
    for (auto v : tokenStore) {
        std::cout << "id: " << std::get<0>(v) << ", s: " << std::get<1>(v) << '\n';
    }
    */

    REQUIRE(tokenStore.size() == 12);

    REQUIRE((std::get<0>(tokenStore[0]) == 65536)); // here, the default id for the white space token
                                                    // is 65536.

    REQUIRE((std::get<1>(tokenStore[0]) == "   ")); // the firt white space token comprised of 3 space chars

    // the first non-white-space token
    REQUIRE((std::get<0>(tokenStore[1]) == '+'));
    REQUIRE((std::get<1>(tokenStore[1]) == "+"));
}

//==============================================================================
namespace
{
    enum token_id
    {
        ID_PLUS = 1024,
        ID_MINUS,
        ID_MUL,
        ID_DIV,
        ID_LPAREN,
        ID_RPAREN
    };

    template <typename Lexer>
    struct op_token_with_explicit_token_id : lex::lexer<Lexer>
    {
        op_token_with_explicit_token_id()
        {
            this->self.add
                        ('+', ID_PLUS)
                        ('-', ID_MINUS)
                        ('*', ID_MUL)
                        ('/', ID_DIV)
                        ('(', ID_LPAREN)
                        (')', ID_RPAREN);
        }
    };
}   // un-named namespace

TEST_CASE("Spirit.Lex getting token info with explicit token id", "[lexer]")
{
    char const * pBegin = "+-*/()"; // no spaces in the source string.
    char const * pEnd = pBegin + std::strlen(pBegin);

    using lexer_t = lex::lexertl::lexer<>;
    using token_store_t = std::vector<std::pair<int, std::string>>;

    op_token_with_explicit_token_id<lexer_t> tokens;
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

    REQUIRE((std::get<0>(tokenStore[0]) == ID_PLUS));
    REQUIRE((std::get<1>(tokenStore[0]) == "+"));
    REQUIRE((std::get<0>(tokenStore[1]) == ID_MINUS));
    REQUIRE((std::get<1>(tokenStore[1]) == "-"));
    REQUIRE((std::get<0>(tokenStore[2]) == ID_MUL));
    REQUIRE((std::get<1>(tokenStore[2]) == "*"));
    REQUIRE((std::get<0>(tokenStore[3]) == ID_DIV));
    REQUIRE((std::get<1>(tokenStore[3]) == "/"));
    REQUIRE((std::get<0>(tokenStore[4]) == ID_LPAREN));
    REQUIRE((std::get<1>(tokenStore[4]) == "("));
    REQUIRE((std::get<0>(tokenStore[5]) == ID_RPAREN));
    REQUIRE((std::get<1>(tokenStore[5]) == ")"));
}

//==============================================================================
namespace
{
    template <typename Lexer>
    struct op_token_with_white_space_state : lex::lexer<Lexer>
    {
        op_token_with_white_space_state()
        {
            this->self = lex::char_('+')
                            | '-'
                            | '*'
                            | '/'
                            | '('
                            | ')';

            white_space_ = "[ \\t\\n]+";
            this->self("WS") = white_space_;    // NOTE: this is for the Spirit.Qi.
        }

        lex::token_def<> white_space_;
    };
}   // un-named namespace

TEST_CASE("Spirit.Lex getting default token info with white-space-state", "[lexer]")
{
    // NOTE: this string has 5 white space tokens.
    char const * pBegin = R"(+   -   * /
                                ()
                            )";
    char const * pEnd = pBegin + std::strlen(pBegin);

    using lexer_t = lex::lexertl::lexer<>;
    using token_store_t = std::vector<std::pair<int, std::string>>;

    op_token_with_white_space_state<lexer_t> tokens;
    token_store_t tokenStore;

    REQUIRE_FALSE(
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

    REQUIRE(tokenStore.size() == 1);
    REQUIRE(std::strstr(pBegin, "   -   * /") == pBegin);
}
