#pragma once
#include <sstream>
template<class Elem>
using tstring=std::basic_string<Elem,std::char_traits<Elem>,
                          std::allocator<Elem>>;

template <class Elem>
using tstringstream=std::basic_stringstream<Elem,
std::char_traits<Elem>,std::allocator<Elem>>;

template<typename Elem>
inline std::vector<tstring<Elem>> split(tstring<Elem> text,
                                   Elem const delimiter)
 {
     auto sstr=tstringstream<Elem>{text};
     auto tokens=std::vector<tstring<Elem>>{};
     auto token=tstring<Elem>{};
     while(std::getline(sstr,token,delimiter))
     {
         if(!token.empty())tokens.push_back(token);
     }

     return tokens;
 }

template<typename Elem>
inline std::vector<tstring<Elem>> split(tstring<Elem> text,
                                   tstring<Elem> const & delimiters)

{

    auto tokens = std::vector<tstring<Elem>>{};
    size_t pos, prev_pos=0;
    while((pos=text.find_first_of(delimiters,prev_pos))!= std::string::npos)
    {
        if(pos>prev_pos)
            tokens.push_back(text.substr(prev_pos,pos-prev_pos));
         prev_pos=pos+1;
    }
    if(prev_pos <text.size())
    tokens.push_back(text.substr(prev_pos,std::string::npos));
    return tokens;

}

inline std::string trim(std::string_view text)//удаляет все пробелы в начале и в конце строки.
{
    auto first{ text.find_first_not_of(' ')};
    auto last {text.find_last_not_of(' ')};
    return text.substr(first,(last-first+1)).data();

}
inline std::vector<std::string> split_to(std::string& str)//разделяет строку по запятым
{

   std::vector<std::string> result;
    auto first=str.begin();
    auto last= str.end();
    while(true)
    {
      auto it=find(first,last,',');
        result.push_back(std::string(first,it));

    if(it==last)
   { break;}
    else{
        first=it+1;
    }

    }
    return result;

}
