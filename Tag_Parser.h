#ifndef DATA_DOM_PARSER_HHHH____29374___
#define DATA_DOM_PARSER_HHHH____29374___

#include "Tag_Parser_Raw.h"

#include <cstddef>
#include <memory>

    //Interface only
template <
    typename InterpreterType    = std::string,
    typename Key                = std::string,
    typename StringType         = std::string
>
class Tag_Parser{
    public:
    //Type aliases
        using raw_type      = Tag_Parser_Raw<InterpreterType, Key, StringType>;
        using value_type    = typename raw_type::value_type;
        using attr_type     = typename raw_type::key_type;
        using str_type      = typename raw_type::str_type;
        using storage_type  = typename raw_type::storage_type;
        using istream_type  = typename raw_type::istream_type;

    //Read only
        const str_type& markup_language()const;

        const str_type& tag()const;

            //Does it have a certain tag?
        bool has(const str_type& test_tag, const str_type& test_lang)const;

        bool has(const str_type& test_tag)const;

        bool has_attr(const str_type& attr)const;

        bool has_bookmark()const;

        std::size_t tags_available(const str_type& test_lang)const;

        std::size_t tags_available()const;

        std::ptrdiff_t available()const;

        std::ptrdiff_t index()const;

        virtual value_type get(
            const str_type& ttag,
            std::ptrdiff_t i,
            const attr_type& attr
        );

        virtual value_type get(std::ptrdiff_t i, const attr_type& attr)const;

        virtual value_type get(const attr_type& attr)const;

        virtual str_type string(const str_type& ttag, std::ptrdiff_t i);

        virtual str_type string(std::ptrdiff_t i)const;

        virtual str_type string()const;

        const raw_type& raw()const;

    //Setting modifiers
        bool markup_language(const str_type& new_lang);

            //Return whether or not the tag was successfully changed
        bool tag(const str_type& new_tag, const str_type& test_lang);

        bool tag(const str_type& new_tag);

        void reset_tag();

        void bookmark();

        void load_bookmark();

        void index(std::ptrdiff_t i);

    //Loading
        void load(istream_type& src);

        template <typename... IStream_Pack>
            void load(
                istream_type& src1,
                istream_type& src2,
                IStream_Pack&... srcn
            );

        void load(str_type& src);

        template <typename... Str_Pack>
            void load(
                str_type& src1,
                str_type& src2,
                Str_Pack&... srcn
            );

        void load_from_file(const str_type& path);

        template <typename... Str_Pack>
            void load_from_file(
                const str_type& path,
                const str_type& path2,
                const Str_Pack&... pathn
            );

    //Erasing and dumping
            //Erases content with current tag
        virtual void erase();

        virtual void dump();

            //Optionally dump all the information into an
            //  external storage space.
        virtual void dump_to(storage_type& collector);

        virtual void raw(raw_type&& new_parser);

    //Constructors and destructor
        Tag_Parser(const Tag_Parser&)             = delete;
        Tag_Parser(Tag_Parser&&)                  = delete;
        Tag_Parser& operator=(const Tag_Parser&)  = delete;
        Tag_Parser& operator=(Tag_Parser&&)       = delete;
        virtual ~Tag_Parser(){}

    protected:
        struct bookmark_type{
            str_type                        m_lang;
            str_type                        m_tag;
            typename raw_type::range_type   m_range;
            typename raw_type::iter_type    m_ptr;

            bookmark_type();
        };

        raw_type                            m_raw;
        bookmark_type                       m_place;
        std::unique_ptr<bookmark_type>      m_bookmark;

            //Optional override
        virtual void initial_load();

        Tag_Parser();

        template <typename... Str_Pack>
            Tag_Parser(const str_type& file1, const Str_Pack&... filen);
};

#include "Tag_Parser.inl"

#endif