#include <iterator>
#include <utility>

#define TP_TEMPL_   \
    template <typename InterpreterType, typename Key, typename StringType>
#define TP_INST_    Tag_Parser<InterpreterType, Key, StringType>

TP_TEMPL_
auto TP_INST_::markup_language()const -> const str_type&
    {return m_place.m_lang;}

TP_TEMPL_
auto TP_INST_::tag()const -> const str_type&
    {return m_place.m_tag;}

TP_TEMPL_
bool TP_INST_::has
    (const str_type& test_tag, const str_type& test_lang)const
{return m_raw.has(test_tag, test_lang);}

TP_TEMPL_
bool TP_INST_::has(const str_type& test_tag)const
    {return m_raw.has(test_tag, m_place.m_lang);}

TP_TEMPL_
bool TP_INST_::has_attr(const str_type& attr)const{
    const auto& packet = m_place.m_ptr->first;
    if(packet.find(attr) != packet.cend())
        return true;
    return false;
}

TP_TEMPL_
bool TP_INST_::has_bookmark()const
    {return m_bookmark != nullptr;}

TP_TEMPL_
std::size_t TP_INST_::tags_available
    (const str_type& test_lang)const
{return m_raw.size(test_lang);}

TP_TEMPL_
std::size_t TP_INST_::tags_available()const
    {return m_raw.size(m_place.m_lang);}

TP_TEMPL_
std::ptrdiff_t TP_INST_::available()const
    {return std::distance(m_place.m_range.first, m_place.m_range.second);}

TP_TEMPL_
std::ptrdiff_t TP_INST_::index()const
    {return std::distance(m_place.m_range.first, m_place.m_ptr);}

TP_TEMPL_
auto TP_INST_::get(
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

TP_TEMPL_
auto TP_INST_::get
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

TP_TEMPL_
auto TP_INST_::get(const attr_type& attr)const -> value_type {
    auto iter = m_place.m_ptr->second.first.find(attr);
    if(iter != m_place.m_ptr->second.first.cend())
        return iter->second;
    return value_type(str_type());
}

TP_TEMPL_
auto TP_INST_::string
    (const str_type& ttag, std::ptrdiff_t i)
-> str_type {
    str_type temp = m_place.m_tag;
    this->tag(ttag);
    value_type toreturn = this->string(i);
    m_place.m_tag = std::move(temp);
    return toreturn;
}

TP_TEMPL_
auto TP_INST_::string(std::ptrdiff_t i)const -> str_type {
    auto fiter = m_place.m_range.first;
    if(i >= this->available())
        return str_type();
    std::advance(fiter, i);
    return fiter->second.second;
}

TP_TEMPL_
auto TP_INST_::string()const -> str_type
    {return m_place.m_ptr->second.second;}

TP_TEMPL_
auto TP_INST_::raw()const -> const raw_type&
    {return m_raw;}

TP_TEMPL_
bool TP_INST_::markup_language(const str_type& new_lang){
    if(!m_raw.has_language(new_lang))
        return false;
    m_place.m_lang = new_lang;
    this->reset_tag();
    return true;
}

TP_TEMPL_
bool TP_INST_::tag
    (const str_type& new_tag, const str_type& test_lang)
{
    if(!this->has(new_tag))
        return false;
    else if(new_tag.empty()){
        this->reset_tag();
        return true;
    }
    m_place.m_tag = new_tag;
    m_place.m_range = m_raw.get(m_place.m_tag, test_lang);
    m_place.m_ptr = m_place.m_range.first;
    return true;
}

TP_TEMPL_
bool TP_INST_::tag(const str_type& new_tag)
    {return this->tag(new_tag, m_place.m_lang);}

TP_TEMPL_
void TP_INST_::reset_tag(){
    m_place.m_tag.clear();
    m_place.m_range = typename raw_type::range_type();
    m_place.m_ptr = typename raw_type::iter_type();
}

TP_TEMPL_
void TP_INST_::bookmark(){
    m_bookmark.reset(new bookmark_type(m_place));
}

TP_TEMPL_
void TP_INST_::load_bookmark(){
    if(this->has_bookmark()){
        std::swap(*m_bookmark, m_place);
        m_bookmark.reset();
    }
}

TP_TEMPL_
void TP_INST_::index(std::ptrdiff_t i){
    m_place.m_ptr = m_place.m_range.first;
    std::advance(m_place.m_ptr, i);
}

TP_TEMPL_
void TP_INST_::load(istream_type& src)
    {m_raw.load(src);}

TP_TEMPL_
template <typename... IStream_Pack>
void TP_INST_::load(
    istream_type& src1,
    istream_type& src2,
    IStream_Pack&... srcn
){m_raw.load(src1, src2, srcn...);}

TP_TEMPL_
void TP_INST_::load(str_type& src)
    {m_raw.load(src);}

TP_TEMPL_
template <typename... Str_Pack>
void TP_INST_::load(
    str_type& src1,
    str_type& src2,
    Str_Pack&... srcn
){m_raw.load(src1, src2, srcn...);}

TP_TEMPL_
void TP_INST_::load_from_file(const str_type& path)
    {m_raw.load_from_file(path);}

TP_TEMPL_
template <typename... Str_Pack>
void TP_INST_::load_from_file(
    const str_type& path,
    const str_type& path2,
    const Str_Pack&... pathn
){m_raw.load_from_file(path, path2, pathn...);}

TP_TEMPL_
void TP_INST_::erase(){
    m_raw.erase(m_place.m_tag, m_place.m_lang);
    m_place.m_tag.clear();
}

TP_TEMPL_
void TP_INST_::dump()
    {m_raw.dump();}

TP_TEMPL_
void TP_INST_::dump_to(storage_type& collector)
    {m_raw.dump_to(collector);}

TP_TEMPL_
void TP_INST_::raw(raw_type&& new_parser)
    {m_raw = std::move(new_parser);}

TP_TEMPL_
TP_INST_::bookmark_type::bookmark_type()
    : m_lang("xml")
    , m_tag("")
    , m_range()
    , m_ptr()
{}

TP_TEMPL_
void TP_INST_::initial_load(){}

TP_TEMPL_
TP_INST_::Tag_Parser()
    : m_raw()
    , m_place()
    , m_bookmark(nullptr)
{}

TP_TEMPL_
template <typename... Str_Pack>
TP_INST_::Tag_Parser
    (const str_type& file1, const Str_Pack&... filen)
    : m_raw(file1, filen...)
    , m_place()
    , m_bookmark(nullptr)
{}

#undef TP_TEMPL_
#undef TP_INST_