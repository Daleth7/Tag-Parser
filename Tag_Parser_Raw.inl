#include <fstream>
#include <cctype>
#include <limits>
#include <sstream>
#include <utility>

template <typename T, typename Key, typename StringType>
bool Tag_Parser_Raw<T, Key, StringType>::empty(const str_type& lang)const
    {return m_storage.find(lang)->second.empty();}

template <typename T, typename Key, typename StringType>
auto Tag_Parser_Raw<T, Key, StringType>::get(
    const key_type& key,
    const str_type& lang
)const -> range_type{
    return  this->has(key, lang)
            ? m_storage.find(lang)->second.equal_range(key)
            : range_type(m_storage.find(lang)->second.cend()
                , m_storage.find(lang)->second.cend())
    ;
}

template <typename T, typename Key, typename StringType>
bool Tag_Parser_Raw<T, Key, StringType>::has(
    const key_type& k,
    const str_type& lang
)const{
    return
        this->has_language(lang) &&
        m_storage.find(lang)->second.find(k)
            != m_storage.find(lang)->second.cend()
    ;
}

template <typename T, typename Key, typename StringType>
bool Tag_Parser_Raw<T, Key, StringType>::has_language
    (const str_type& test_lang)const
{return m_storage.find(test_lang) != m_storage.cend();}

template <typename T, typename Key, typename StringType>
std::size_t Tag_Parser_Raw<T, Key, StringType>::size(const str_type& lang)const{
    auto iter = m_storage.find(lang);
    return iter != m_storage.cend() ? iter->second.size() : 0;
}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::load
    (std::istream& data)
throw(std::runtime_error){
    str_type markup_language;
    auto tolower = [](str_type& s)
                    {for(auto& ch : s) ch = std::tolower(ch);};
    while(true){
        markup_language.clear();
        data >> std::ws;
        char start = data.get();
        if(data.fail())
            return;
        if(start != '<')
            throw std::runtime_error("Input is in an invalid format.");
        std::getline(data, markup_language, '>');
        if(markup_language.front() == '!')
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

template <typename T, typename Key, typename StringType>
template <typename... IStream_Pack>
void Tag_Parser_Raw<T, Key, StringType>::load(
    std::istream& src1,
    std::istream& src2,
    IStream_Pack&... srcn
)throw(std::runtime_error){
    this->load(src1);
    this->load(src2, srcn...);
}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::load_from_file(const str_type& filepath)
    throw(std::runtime_error)
{
    std::ifstream data(filepath);
    if(!data.is_open())
        throw std::runtime_error("Could not open " + filepath);
    this->load(data);
}

template <typename T, typename Key, typename StringType>
template <typename... Str_Pack>
void Tag_Parser_Raw<T, Key, StringType>::load_from_file(
    const str_type& file1,
    const str_type& file2,
    const Str_Pack&... filen
) throw(std::runtime_error){
    this->load_from_file(file1);
    this->load_from_file(file2, filen...);
}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::erase
    (const key_type& key, const str_type& lang)
{
    auto iter = m_storage.find(lang);
    if(iter != m_storage.end())
        iter->second.erase(key);
}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::dump()
    {m_storage.clear();}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::dump(const str_type& lang){
    auto iter = m_storage.find(lang);
    if(iter != m_storage.cend())
        iter->second.clear();
}

template <typename T, typename Key, typename StringType>
void Tag_Parser_Raw<T, Key, StringType>::dump_to(storage_type& collector){
    std::swap(collector, m_storage);
    m_storage.clear();
}

template <typename T, typename Key, typename StringType>
template <typename... Str_Pack>
Tag_Parser_Raw<T, Key, StringType>::Tag_Parser_Raw(
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

template <typename T, typename Key, typename StringType>
bool Tag_Parser_Raw<T, Key, StringType>::load_tags(
    const str_type& lang,
    tag_pack_type& stor,
    std::istream& data
){
    auto tolower = [](str_type& s)
                    {for(auto& ch : s) ch = std::tolower(ch);};
    const std::streamsize lim = std::numeric_limits<std::streamsize>::max();
    while(data.ignore(lim, '<')){
        str_type key_str;
        data >> key_str;
        bool get_values(true);
        if(key_str.back() == '>'){
            key_str.pop_back();
            get_values = false;
        }
        tolower(key_str);
        if(key_str.front() == '/'){
            if(key_str.substr(1) == lang)   return (data >> std::ws);
            else                            continue;
        }else if(key_str.front() == '!')    continue;

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
                if(
                    !std::getline(data, text, '<')      ||
                    !data.ignore(lim, '>')
                )                           break;

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