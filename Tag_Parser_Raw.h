#ifndef DATA_PARSER_H_HHH____RAW_0009435_
#define DATA_PARSER_H_HHH____RAW_0009435_

#include "Tag_Parser_Default_Interpreter.h"

#include <map>
#include <unordered_map>
#include <stdexcept>
#include <string>

namespace std{
    template <typename T1, typename T2>
    class pair;

    template <typename CharT>
    class char_traits;

    template <typename CharT, typename Traits>
    class basic_istream;

    typedef basic_istream<char, char_traits<char> > istream;
}

template <
    typename T          = Tag_Parser_Default_Interpreter,
    typename Key        = std::string,
    typename StringType = std::string
>
class Tag_Parser_Raw{
    public:
        using value_type    = T;
        using key_type      = Key;
        using str_type      = StringType;
        using packet_type   = std::map<key_type, value_type>;
        using pair_type     = std::pair<packet_type, str_type>;
        using tag_pack_type = std::unordered_multimap<key_type, pair_type>;
        using storage_type  = std::unordered_map<str_type, tag_pack_type>;
        using iter_type     = typename tag_pack_type::const_iterator;
        using range_type    = std::pair<iter_type, iter_type>;

        bool empty(const str_type& = "xml")const;
            //If the key is not present, returns a pair of an empty container
            //  and empty string.
        range_type get(const key_type&, const str_type& = "xml")const;
        bool has(const key_type&, const str_type& = "xml")const;
        bool has_language(const str_type&)const;
        std::size_t size(const str_type& = "xml")const;

            //Calling load() will not dump existing data
        void load(std::istream& filepath) throw(std::runtime_error);
            //For loading from more than one source
        template <typename... IStream_Pack>
        void load(
            std::istream&,
            std::istream&,
            IStream_Pack&...
        )throw(std::runtime_error);
        void load_from_file(const str_type& filepath) throw(std::runtime_error);
        template <typename... Str_Pack>
        void load_from_file(const str_type&, const str_type&, const Str_Pack&...)
             throw(std::runtime_error);
        void erase(const key_type&, const str_type& = "xml");
            //Dump everything
        void dump();
            //Dump tags of a specific language
        void dump(const str_type&);
            //Optionally dump all the information into an
            //  external storage space.
        void dump_to(storage_type&);

            //For loading files upon construction
        template <typename... Str_Pack>
        Tag_Parser_Raw(const str_type&, const Str_Pack&...);

        Tag_Parser_Raw()                                   = default;
        Tag_Parser_Raw(const Tag_Parser_Raw&)             = delete;
        Tag_Parser_Raw(Tag_Parser_Raw&&)                  = default;
        Tag_Parser_Raw& operator=(const Tag_Parser_Raw&)  = delete;
        Tag_Parser_Raw& operator=(Tag_Parser_Raw&&)       = default;
        ~Tag_Parser_Raw()                                  = default;

    protected:
        bool load_tags(const str_type&, tag_pack_type&, std::istream&);

    private:
        storage_type m_storage;
};

#include "Tag_Parser_Raw.inl"

#endif