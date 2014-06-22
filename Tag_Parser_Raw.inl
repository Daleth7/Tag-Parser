#include <fstream>
#include <cctype>
#include <limits>
#include <sstream>
#include <utility>

#define TPR_TEMPL_  \
    template <typename InterpreterType, typename Key, typename StringType>
#define TPR_INST_   Tag_Parser_Raw<InterpreterType, Key, StringType>

TPR_TEMPL_
bool TPR_INST_::empty(const str_type& lang)const
    {return m_storage.find(lang)->second.empty();}

TPR_TEMPL_
auto TPR_INST_::get(
    const key_type& key,
    const str_type& lang
)const -> range_type{
    return  this->has(key, lang)
            ? m_storage.find(lang)->second.equal_range(key)
            : range_type(m_storage.find(lang)->second.cend()
                , m_storage.find(lang)->second.cend())
    ;
}

TPR_TEMPL_
bool TPR_INST_::has(
    const key_type& k,
    const str_type& lang
)const{
    return
        this->has_language(lang) &&
        m_storage.find(lang)->second.find(k)
            != m_storage.find(lang)->second.cend()
    ;
}

TPR_TEMPL_
bool TPR_INST_::has_language
    (const str_type& test_lang)const
{return m_storage.find(test_lang) != m_storage.cend();}

TPR_TEMPL_
std::size_t TPR_INST_::size(const str_type& lang)const{
    auto iter = m_storage.find(lang);
    return iter != m_storage.cend() ? iter->second.size() : 0;
}

TPR_TEMPL_
void TPR_INST_::load
    (istream_type& data)
throw(std::runtime_error){
    str_type markup_language;
    auto tolower = [](str_type& s)
                    {for(auto& ch : s) ch = std::tolower(ch);};
    while(true){
        //First grab the language tag. Load any sub-tags and repeat.
        markup_language.clear();
        data >> std::ws;
        const char start = data.get();
        if(data.fail())
            return;
        if(start != '<')
            throw std::runtime_error("Input is in an invalid format.");
        std::getline(data, markup_language, '>');
        if(is_comment(markup_language, data))
            continue;
        tolower(markup_language);
            //Check this inside the body so that any continue statements
            //  don't accidentally call load_tags().
        if(
            data.fail() ||
            !this->load_tags(markup_language, m_storage[markup_language], data)
        )   return;
    }
}

TPR_TEMPL_
template <typename... IStream_Pack>
void TPR_INST_::load(
    istream_type& src1,
    istream_type& src2,
    IStream_Pack&... srcn
)throw(std::runtime_error){
    this->load(src1);
    this->load(src2, srcn...);
}

TPR_TEMPL_
void TPR_INST_::load(const str_type& src)
    throw(std::runtime_error)
{
    std::basic_istringstream<typename str_type::value_type> data(src);
    this->load(data);
}

TPR_TEMPL_
template <typename... Str_Pack>
void TPR_INST_::load(
    const str_type& src1,
    const str_type& src2,
    const Str_Pack&... srcn
) throw(std::runtime_error){
    this->load(src1);
    this->load(src2, srcn...);
}

TPR_TEMPL_
void TPR_INST_::load_from_file(const str_type& filepath)
    throw(std::runtime_error)
{
    std::basic_ifstream<typename str_type::value_type> data(filepath);
    if(!data.is_open())
        throw std::runtime_error("Could not open " + filepath);
    this->load(data);
}

TPR_TEMPL_
template <typename... Str_Pack>
void TPR_INST_::load_from_file(
    const str_type& file1,
    const str_type& file2,
    const Str_Pack&... filen
) throw(std::runtime_error){
    this->load_from_file(file1);
    this->load_from_file(file2, filen...);
}

TPR_TEMPL_
void TPR_INST_::erase
    (const key_type& key, const str_type& lang)
{
    auto iter = m_storage.find(lang);
    if(iter != m_storage.end())
        iter->second.erase(key);
}

TPR_TEMPL_
void TPR_INST_::dump()
    {m_storage.clear();}

TPR_TEMPL_
void TPR_INST_::dump(const str_type& lang){
    auto iter = m_storage.find(lang);
    if(iter != m_storage.cend())
        iter->second.clear();
}

TPR_TEMPL_
void TPR_INST_::dump_to(storage_type& collector){
    std::swap(collector, m_storage);
    m_storage.clear();
}

TPR_TEMPL_
template <typename... Str_Pack>
TPR_INST_::Tag_Parser_Raw(
    const str_type& file1,
    const Str_Pack&... filen
)
    : m_storage()
{this->load_from_file(file1, filen...);}

#include <type_traits>
namespace Tag_Parser_Raw_Helper{
    template <typename StringType, typename CondType, typename InterpreterType>
    struct assign_value{
        static void assign(
            const StringType& s,
            const StringType& attr_name,
            InterpreterType& i
        ){i = InterpreterType(attr_name, s);}
    };

    template <typename StringType, typename CondType, typename T>
    struct assign_value<StringType, CondType, std::basic_string<T> >{
        static void assign(
            const StringType& s,
            const StringType&,
            std::basic_string<T>& i
        ){i = s;}
    };

    template <typename StringType, typename InterpreterType>
    struct assign_value<StringType, std::true_type, InterpreterType>{
        static void assign(
            const StringType& s,
            const StringType&,
            InterpreterType& i
        ){
            std::basic_istringstream<typename StringType::value_type> ss(s);
            ss >> i;
        }
    };

    template <typename StringType, typename InterpreterType>
    void choose_communication(
        const StringType& s,
        const StringType& attr_name,
        InterpreterType& i
    ){
        assign_value<
            StringType,
            typename std::is_fundamental<InterpreterType>::type,
            InterpreterType
            >::assign(s, attr_name, i);
    }
}

TPR_TEMPL_
bool TPR_INST_::load_tags(
    const str_type& lang,
    tag_pack_type& stor,
    istream_type& data
){
    auto tolower = [](str_type& s)
                    {for(auto& ch : s) ch = std::tolower(ch);};
    const std::streamsize lim = std::numeric_limits<std::streamsize>::max();
    while(data.ignore(lim, '<')){
        //Grab the tag name
        str_type key_str;
        data >> key_str;
        bool get_values(true);
        if(key_str.back() == '>'){
            key_str.pop_back();
            get_values = false;
        }
        tolower(key_str);
        //If this turns out to be a closing tag, ignore it and move on
        if(key_str.front() == '/'){
            if(key_str.substr(1) == lang)   return (data >> std::ws);
            else                            continue;
            //Turns out to be a comment. Ignore.
        }else if(is_comment(key_str, data)) continue;

        str_type attr_str, text;
        packet_type packet;
        bool look_for_text(true);
        if(get_values){
            if(
                !std::getline(data, attr_str, '>')  ||
                !(data >> std::ws)
            )                               break;

            if(!(look_for_text = attr_str.back() != '/'))
                attr_str.pop_back();

            std::basic_istringstream<typename str_type::value_type> iss(attr_str);
            while(
                (iss >> std::ws)                    &&
                std::getline(iss, attr_str, '=')    &&
                (iss >> std::ws)
            ){
                std::size_t pos(attr_str.find(' '));
                while(pos != str_type::npos){
                    attr_str.erase(pos, 1);
                    pos = attr_str.find(' ');
                }
                str_type value_str;
                iss >> value_str;
                if(value_str.back() == '>'){
                    value_str.pop_back();
                    iss.unget();
                }
                if(value_str.front() == '\"'){
                    if(value_str.back() != '\"'){
                        str_type end;
                        std::getline(iss, end, '\"');
                        value_str += end;
                    }
                    value_str.erase(0, 1);
                    if(value_str.back() == '\"')
                        value_str.pop_back();
                }
                tolower(value_str);
                tolower(attr_str);
                value_type interpreter;
                Tag_Parser_Raw_Helper::choose_communication
                    (value_str, attr_str, interpreter);
                packet.insert({attr_str, interpreter});
            }

            if(look_for_text){
                text = this->read_tag_contents("</" + key_str, data);
                if(data.fail())             break;
                while(std::isspace(text.back()))
                    text.pop_back();
                std::size_t pos(text.find('\n'));
                while(pos != str_type::npos){
                    text.erase(pos, 1);
                    pos = text.find('\n');
                }
                pos = text.find('\t');
                while(pos != str_type::npos){
                    text.erase(pos, 1);
                    pos = text.find('\t');
                }
                pos = text.find("  ");
                while(pos != str_type::npos){
                    text.erase(pos, 1);
                    pos = text.find("  ");
                }
            }
        }

        stor.insert({key_type(key_str), {packet, text}});
    }

    return data;
}

TPR_TEMPL_
auto TPR_INST_::read_tag_contents(
    const str_type& tag_closer,
    istream_type& src
) -> str_type{
    str_type toreturn;
    std::getline(src, toreturn, '>');
    auto not_found_closer = [&toreturn](const str_type& cl){
            str_type end(toreturn.substr(toreturn.size()-cl.size()));
            for(auto& ch : end) ch = std::tolower(ch);
            return end != cl;
        };
    while(not_found_closer(tag_closer) && src.good()){
        str_type tail;
        std::getline(src, tail, '>');
        toreturn += ">" + tail;
    }
    std::size_t pos(toreturn.find_last_of('/'));
    if(pos != str_type::npos)
        toreturn.erase(pos-1);
    return toreturn;
}

TPR_TEMPL_
bool TPR_INST_::is_comment(const str_type& testee, istream_type& data){
    if(testee.find("!--") == 0){
        this->read_tag_contents("--", data);
        return true;
    }
    return false;
}

#undef TPR_TEMPL_
#undef TPR_INST_