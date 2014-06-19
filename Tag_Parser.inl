#include <iterator>
#include <utility>

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::markup_language()const -> const str_type&
    {return m_place.m_lang;}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::tag()const -> const str_type&
    {return m_place.m_tag;}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::has
    (const str_type& test_tag, const str_type& test_lang)const
{return m_parser.has(test_tag, test_lang);}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::has(const str_type& test_tag)const
    {return m_parser.has(test_tag, m_place.m_lang);}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::has_attr(const str_type& attr)const{
    const auto& packet = m_place.m_ptr->first;
    if(packet.find(attr) != packet.cend())
        return true;
    return false;
}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::has_bookmark()const
    {return m_bookmark != nullptr;}

template <typename T, typename Key, typename StringType>
std::size_t Tag_Parser<T, Key, StringType>::tags_available
    (const str_type& test_lang)const
{return m_parser.size(test_lang);}

template <typename T, typename Key, typename StringType>
std::size_t Tag_Parser<T, Key, StringType>::tags_available()const
    {return m_parser.size(m_place.m_lang);}

template <typename T, typename Key, typename StringType>
std::ptrdiff_t Tag_Parser<T, Key, StringType>::available()const
    {return std::distance(m_place.m_range.first, m_place.m_range.second);}

template <typename T, typename Key, typename StringType>
std::ptrdiff_t Tag_Parser<T, Key, StringType>::index()const
    {return std::distance(m_place.m_range.first, m_place.m_ptr);}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::get(
    const str_type& ttag,
    std::ptrdiff_t i,
    const attr_type& attr
) -> value_type {
    str_type temp = m_place.m_tag;
    this->tag(ttag);
    value_type toreturn = this->get(i, attr);
    m_place.m_tag = std::move(temp);
    return toreturn;
}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::get
    (std::ptrdiff_t i, const attr_type& attr)const
-> value_type {
    if(i >= this->available())
        return value_type(str_type());
    auto fiter = m_place.m_range.first;
    std::advance(fiter, i);
    auto iter = fiter->second.first.find(attr);
    if(iter != fiter->second.first.cend())
        return iter->second;
    return value_type(str_type());
}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::get(const attr_type& attr)const -> value_type {
    auto iter = m_place.m_ptr->second.first.find(attr);
    if(iter != m_place.m_ptr->second.first.cend())
        return iter->second;
    return value_type(str_type());
}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::string
    (const str_type& ttag, std::ptrdiff_t i)
-> str_type {
    str_type temp = m_place.m_tag;
    this->tag(ttag);
    value_type toreturn = this->string(i);
    m_place.m_tag = std::move(temp);
    return toreturn;
}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::string(std::ptrdiff_t i)const -> str_type {
    auto fiter = m_place.m_range.first;
    if(i >= this->available())
        return str_type();
    std::advance(fiter, i);
    return fiter->second.second;
}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::string()const -> str_type
    {return m_place.m_ptr->second.second;}

template <typename T, typename Key, typename StringType>
auto Tag_Parser<T, Key, StringType>::raw()const -> const parser_type&
    {return m_parser;}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::markup_language(const str_type& new_lang){
    if(!m_parser.has_language(new_lang))
        return false;
    m_place.m_lang = new_lang;
    this->reset_tag();
    return true;
}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::tag
    (const str_type& new_tag, const str_type& test_lang)
{
    if(!this->has(new_tag))
        return false;
    else if(new_tag.empty()){
        this->reset_tag();
        return true;
    }
    m_place.m_tag = new_tag;
    m_place.m_range = m_parser.get(m_place.m_tag, test_lang);
    m_place.m_ptr = m_place.m_range.first;
    return true;
}

template <typename T, typename Key, typename StringType>
bool Tag_Parser<T, Key, StringType>::tag(const str_type& new_tag)
    {return this->tag(new_tag, m_place.m_lang);}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::reset_tag(){
    m_place.m_tag.clear();
    m_place.m_range = typename parser_type::range_type();
    m_place.m_ptr = typename parser_type::iter_type();
}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::bookmark(){
    m_bookmark.reset(new bookmark_type(m_place));
}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::load_bookmark(){
    if(this->has_bookmark()){
        std::swap(*m_bookmark, m_place);
        m_bookmark.reset();
    }
}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::index(std::ptrdiff_t i){
    m_place.m_ptr = m_place.m_range.first;
    std::advance(m_place.m_ptr, i);
}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::load(const str_type& path)
    {m_parser.load(path);}

template <typename T, typename Key, typename StringType>
template <typename... Str_Pack>
void Tag_Parser<T, Key, StringType>::load(
    const str_type& path,
    const str_type& path2,
    const Str_Pack&... pathn
){m_parser.load(path, path2, pathn...);}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::erase(){
    m_parser.erase(m_place.m_tag, m_place.m_lang);
    m_place.m_tag.clear();
}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::dump()
    {m_parser.dump();}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::dump_to(storage_type& collector)
    {m_parser.dump_to(collector);}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::raw(parser_type&& new_parser)
    {m_parser = std::move(new_parser);}

template <typename T, typename Key, typename StringType>
Tag_Parser<T, Key, StringType>::bookmark_type::bookmark_type()
    : m_lang("xml")
    , m_tag("")
    , m_range()
    , m_ptr()
{}

template <typename T, typename Key, typename StringType>
void Tag_Parser<T, Key, StringType>::initial_load(){}

template <typename T, typename Key, typename StringType>
Tag_Parser<T, Key, StringType>::Tag_Parser()
    : m_parser()
    , m_place()
    , m_bookmark(nullptr)
{}

template <typename T, typename Key, typename StringType>
template <typename... Str_Pack>
Tag_Parser<T, Key, StringType>::Tag_Parser
    (const str_type& file1, const Str_Pack&... filen)
    : m_parser(file1, filen...)
    , m_place()
    , m_bookmark(nullptr)
{}