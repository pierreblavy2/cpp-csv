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


#ifndef CSV_TRIM_HPP
#define CSV_TRIM_HPP


//  https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring

#include <algorithm>
#include <cctype>
#include <string>


namespace csv{

// trim from start (in place)
template<typename IsNotSpace>
inline void trim_left(
    std::string &s,
    IsNotSpace is_not_space =[](unsigned char ch) {return !std::isspace(ch);}
) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),is_not_space ));
}


inline void trim_left(std::string &s) {
    trim_left(s, [](unsigned char ch) {return !std::isspace(ch);} );
}





// trim from end (in place)
template<typename IsNotSpace>
inline void trim_right(std::string &s, IsNotSpace is_not_space) {
    s.erase(std::find_if(s.rbegin(), s.rend(),is_not_space ).base(), s.end());
}


inline void trim_right(std::string &s) {
    trim_right(s, [](unsigned char ch) {return !std::isspace(ch);} );
}




// trim from both ends (in place)
template<typename IsNotSpace>
inline void trim(  std::string &s, IsNotSpace is_not_space){
    trim_left (s,is_not_space);
    trim_right(s,is_not_space);
}



inline void trim(std::string &s) {
    trim(s, [](unsigned char ch) {return !std::isspace(ch);} );
}



}
#endif // TRIM_HPP
