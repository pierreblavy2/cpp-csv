/*
Copyright (C) 2024 Pierre BLAVY

This program (csv) is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef STR_CAT_HPP
#define STR_CAT_HPP

#include <string>

namespace csv{

//Fn is a function that takes a const Range::value_t, and returns a std::string.
template<typename Range, typename Fn>
void str_cat(std::string &s, const Range&c, const std::string &sep, Fn to_string){
    auto b = std::cbegin(c);
    auto e = std::cend  (c);

    if(b != e){s+=to_string(*b); ++b;}
    while(b!=e){
        s+= sep;
        s+= to_string(*b);
        ++b;
    }
}

template<typename Range>
void str_cat(std::string &s, const Range &c, const std::string &sep){str_cat(s,c,sep,[](const auto &s)->const auto&{return s;});}


//Fn is a function that takes a const Range::value_t, and returns a std::string.
template<typename Range, typename Fn>
void str_cat_n(std::string &s, const Range&c, const std::string &sep, size_t limit, Fn to_string){
    if(limit==0)[[unlikely]]{return;}

    auto b = std::cbegin(c);
    auto e = std::cend  (c);

    if(b != e){s+=to_string(*b); ++b;}
    size_t n = 1;

    while(b!=e){
        if(n==limit)[[unlikely]]{return;}
        ++n;
        s+= sep;
        s+= to_string(*b);
        ++b;
    }
}

template<typename Range>
void str_cat_n(std::string &s, const Range&c, const std::string &sep, size_t limit){
    str_cat_n(s,c,sep,limit,[](const auto &s)->const auto&{return s;}) ;
}


}
#endif // STR_CAT_HPP
