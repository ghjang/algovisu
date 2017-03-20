#ifndef ALGOVISU_CACULATOR_H
#define ALGOVISU_CACULATOR_H


#include <boost/spirit/include/lex_lexertl.hpp>


namespace algovisu
{
    namespace lex = boost::spirit::lex;
    
    template <typename Lexer>
    struct calc_token : lex::lexer<Lexer>
    {

    };
} // namespace algovisu


#endif  // ALGOVISU_CACULATOR_H
