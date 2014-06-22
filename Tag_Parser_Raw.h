#ifndef DATA_PARSER_H_HHH____RAW_0009435_
#define DATA_PARSER_H_HHH____RAW_0009435_

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
}

template <
    typename InterpreterType    = std::string,
    typename Key                = std::string,
    typename StringType         = std::string
>
class Tag_Parser_Raw{
    public:
    //Type aliases
        using value_type    = InterpreterType;
        using key_type      = Key;
        using str_type      = StringType;
        using packet_type   = std::map<key_type, value_type>;
        using pair_type     = std::pair<packet_type, str_type>;
        using tag_pack_type = std::unordered_multimap<key_type, pair_type>;
        using storage_type  = std::unordered_map<str_type, tag_pack_type>;
        using iter_type     = typename tag_pack_type::const_iterator;
        using range_type    = std::pair<iter_type, iter_type>;
        using istream_type  = std::basic_istream<typename str_type::value_type>;

    //Read only
        bool empty(const str_type& = "xml")const;
            //If the key is not present, returns a pair of equivalent iterators.
        range_type get(const key_type&, const str_type& = "xml")const;
        bool has(const key_type&, const str_type& = "xml")const;
        bool has_language(const str_type&)const;
        std::size_t size(const str_type& = "xml")const;

    //Loading
            //Calling load() will not dump existing data
        void load(istream_type& filepath) throw(std::runtime_error);
            //For loading from more than one source
        template <typename... IStream_Pack>
            void load(
                istream_type&,
                istream_type&,
                IStream_Pack&...
            )throw(std::runtime_error);
        void load(const str_type& src)throw(std::runtime_error);
        template <typename... Str_Pack>
            void load(const str_type&, const str_type&, const Str_Pack&...)
                throw(std::runtime_error);
        void load_from_file(const str_type& filepath) throw(std::runtime_error);
        template <typename... Str_Pack>
            void load_from_file(
                const str_type&,
                const str_type&,
                const Str_Pack&...
            )throw(std::runtime_error);

     //Erasing and dumping
        void erase(const key_type&, const str_type& = "xml");
            //Dump everything
        void dump();
            //Dump tags of a specific language
        void dump(const str_type&);
            //Optionally dump all the information into an
            //  external storage space.
        void dump_to(storage_type&);

    //Constructors and destructor
            //For loading files upon construction
        template <typename... Str_Pack>
            Tag_Parser_Raw(const str_type&, const Str_Pack&...);

        Tag_Parser_Raw()                                    = default;
        Tag_Parser_Raw(const Tag_Parser_Raw&)               = delete;
        Tag_Parser_Raw(Tag_Parser_Raw&&)                    = default;
        Tag_Parser_Raw& operator=(const Tag_Parser_Raw&)    = delete;
        Tag_Parser_Raw& operator=(Tag_Parser_Raw&&)         = default;
        ~Tag_Parser_Raw()                                   = default;

    protected:
        bool load_tags(const str_type&, tag_pack_type&, istream_type&);
            //Only reads the rest
        str_type read_tag_contents(const str_type&, istream_type&);
        bool is_comment(const str_type&, istream_type&);

    private:
        storage_type m_storage;
};

#include "Tag_Parser_Raw.inl"

#endif