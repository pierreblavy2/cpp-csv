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

#ifndef CSV_WRITER_PIERRE_HPP
#define CSV_WRITER_PIERRE_HPP

#include <unordered_map>
#include <vector>

#include <string>
#include <ostream>
#include <filesystem>

#include <unordered_map>

#include <cassert>

namespace csv{

//USAGE :
//=== define writer ===
//Csv_writer w;
// w.add_column("city");
// w.add_column("population");
//
// w.set_write("write_here.tsv");
//
//=== write header ===
// w.write_header();
//
//=== write data ===
// NOTE : writing twice to the same place overwrite without asking.
// w.write_token("city","New York"); w.write_token("city","Paris");
// is the same as : w.write_token("city","Paris");
//
//--- write by name (slow) ---
//w.write_token("city"      ,"New York");
//w.write_token("population","8.33 M");
//w.write_endl();
//
//--- write by index (fast) ---
// //once:
// NOTE : indexes can be obatined by capturing the return of add_column
// size_t i_city       = w.get_index("city");
// size_t i_population = w.get_index("population");
//
// //in a loop
// w.write_token(i_city      ,"New York");
// w.write_token(i_population,"8.33 M");
// w.write_endl();
//
//--- write a line, ordered (faster) ---
// w.write_tokens("New York","8.33 M");
// // or  w.write_tokens("New York") ; w.write_tokens("8.33 M");
// w.write_endl();
//
//--- write a full line, ordered (faster) ---
// w.write_line("New York","8.33 M");
//
//=== colse or recycle writer ===
//WARNING : don't forget to call reset or set_write
//if you want to flush data and close ostream before destuctor call
//
//  change destination, don't change column definition
//  w.set_write("write_elsewhere.tsv");
//
//  reset everything including columns
//  w.reset();
//  w.set_write("write_elsewhere.tsv");
//  w.add_column("fruit_name");


class Csv_writer{
public:
    
    Csv_writer(char sep_='\t', char endl_ = '\n');
    ~Csv_writer();

    void set_write(std::ostream &out_, const std::string &name_);
    void set_write(const std::filesystem::path &p);

    template<typename Str> size_t add_column(Str &&col_name);
    size_t get_column(const std::string &col_name)const;

    void write_header();
    void write_endl();

    char sep;
    char endl;

    template<typename Str> void write_token(const std::string & colname, Str &&tok);
    template<typename Str> void write_token(size_t col_index,            Str &&tok);


    // write_tokens(string...)
    void write_tokens(){};
    template<typename Str>                void write_tokens(Str &&s) {line_v[col_count]=std::forward<Str>(s); ++col_count;};
    template<typename... A, typename Str> void write_tokens(Str &&s, A&&... a){write_tokens(std::forward<Str>(s) ); write_tokens(std::forward<A>(a)...);};

    // write_tokens(size_t, string...)
    void write_tokens_at(size_t){};
    template<typename Str>                void write_tokens_at(size_t i, Str&& s){line_v[i]=std::forward<Str>(s);};
    template<typename... A, typename Str> void write_tokens_at(size_t i, Str&& s, A&&... a){write_tokens_at(i, std::forward<Str>(s) ); write_tokens_at(i+1, std::forward<A>(a)...);};

    template<typename... A> void write_line(A&&... a){assert( sizeof...(A)==header_v.size() );   write_line_r<false>(std::forward<A>(a)...); *out << endl ;};


    void reset(); //close if owned, remove columns
    void close(); //close if owned

    private:
    std::string name; //used to produce clear error messages
    std::unordered_map<std::string,size_t> colname_to_index;
    std::vector<std::string> line_v;
    std::vector<const std::string*> header_v;
    size_t line_count=0;
    size_t col_count = 0;
    
    std::ostream * out=nullptr;
    bool own_out = false;
    
    void check();

    //B=true : write separator before, B=false : don't write separator
    template<bool B> void write_line_r(){}
    template<bool B, typename Str >               void write_line_r(Str &&s){if constexpr(B){*out << sep;} *out << std::forward<Str>(s) ; }
    template<bool B, typename Str, typename... A> void write_line_r(Str &&s, A&&... a){ write_line_r<B>( std::forward<Str>(s) ); write_line_r<true>(std::forward<A>(a)...); };

    
};

}




template<typename Str>
size_t csv::Csv_writer::add_column(Str &&col_name){
    size_t r = header_v.size();
    auto tmp = colname_to_index.insert({ std::forward<Str>(col_name) ,r});
    if(!tmp.second){throw std::runtime_error("Error in Csv_writer : duplicated column name, colname="+ tmp.first->first +",index="+std::to_string( header_v.size() )+" name="+name);}
    header_v.push_back(&(tmp.first->first) );
    line_v  .push_back("");
    return r;
}

template<typename Str>
void csv::Csv_writer::write_token(const std::string & colname, Str &&tok){
    auto f=colname_to_index.find(colname);
    if(f==colname_to_index.end()){
        throw std::runtime_error("Error in Csv_writer : wrong column name, colname="+colname+" name="+name);
    }
    line_v[f->second]=std::forward<Str>(tok);
}

template<typename Str>
void csv::Csv_writer::write_token(size_t col_index, Str &&tok){
    line_v[col_index]=std::forward<Str>(tok);
}

#endif
