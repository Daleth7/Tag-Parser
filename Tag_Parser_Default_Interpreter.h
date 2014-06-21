#ifndef TAG_PARSER_DEFAULT_INTERPRETER_H_23833FFF_
#define TAG_PARSER_DEFAULT_INTERPRETER_H_23833FFF_

#include <string>

//Does nothing but illustrates how an interpreter
//  may be built into the parser.
class Tag_Parser_Default_Interpreter{
    public:
        operator std::string const&()const
            {return m_str;}

        Tag_Parser_Default_Interpreter() = default;
        Tag_Parser_Default_Interpreter(const std::string& s)
            : m_str(s)
        {}
    private:
        std::string m_str;
};

#endif