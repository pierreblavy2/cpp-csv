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


#include "Csv_reader.hpp"
#include "tools/str_cat.hpp"



#include <set>
#include <fstream>



//--- helpers ---
namespace {

template<typename Fn> //Fn is any callable as void(size_t, std::string&&);
void tokenize(std::string_view string,  char sep, Fn fn){
    if(string==""){return;}

    std::string token;
    size_t index=0;
    for(char c : string){
        if(c==sep){
            fn(index, std::move(token) );
            token="";
            ++index;
        }else{
            token +=c;
        }
    }
    fn(index,std::move(token) );
}

template<typename Fn> //Fn is any callable as void(size_t, std::string&&);
void tokenize(std::istream &in,  char sep, char endl, Fn fn){
    if(!in){return;}

    std::string token;
    size_t index=0;
    char c;
    while (in.get(c)){
        if(c==endl){fn(index,std::move(token) ); return;}

        if(c==sep){
            fn(index, std::move(token) );
            token="";
            ++index;
        }else{
            token +=c;
        }
    }
    fn(index,std::move(token) );
}








}//end namespace {






void csv::Csv_reader::read_header(std::istream &in){
    fn_vector.resize(0);
    line_count=0;

    std::set<std::string_view> missing_cols;
    for(const auto &p:colname_to_fn){
        missing_cols.emplace(p.first);
    }

    std::set<std::string> seen_cols;
    std::set<std::string> duplicated_cols;


    auto fn_header = [&,this](size_t , std::string &&h){
        at_header(h);

        missing_cols.erase(h);

        if(seen_cols.count(h)!=0){
            duplicated_cols.insert(h);
        }else{
            seen_cols. insert(h);
        }

        auto x=colname_to_fn.find(h);
        if(x==colname_to_fn.end()){
            fn_vector.emplace_back( nullptr); //nothing defined
        }else{
            fn_vector.emplace_back( &x->second );
        }
    };
    tokenize(in,sep,endl,fn_header);

    //missing columns
    if(!missing_cols.empty()){
        std::string err = "Error in Csv_reader::read_header "+ std::to_string(missing_cols.size()) +" columns are missing. "
                                                                                                      "missing =";
        csv::str_cat(err, missing_cols, ", " );
        err+=", found " + std::to_string(seen_cols.size()) +" columns = ";
        csv::str_cat(err, seen_cols, ", " );
        throw std::runtime_error( std::move(err) );
    }

    //duplicated columns
    if(!duplicated_cols.empty()){
        std::string err = "Error in Csv_reader::read_header duplicated columns names. "
                          "duplicated =";
        str_cat(err, duplicated_cols, ", " );
        throw std::runtime_error( std::move(err) );
    }

}


bool csv::Csv_reader::read_line(std::istream &in){
    std::string line_str;
    bool ok= !!std::getline(in,line_str,endl);
    if(!ok)[[unlikely]]{return false;}

    ++line_count;

    auto fn_col=[&,this](size_t col, std::string&& token){
        if(col>= fn_vector.size()){
            std::string err = "Error in Csv_reader::read_line : too many item in line. line="+std::to_string(line_count)+", extra_token="+token;
        }

        //call function if defined
        auto pfn = fn_vector[col];
        if(pfn!=nullptr){
            try{
              at_token(token);
              (*pfn)(line_count,std::move(token));
            }catch(std::exception &e){
                std::string err = "Error in Csv_reader::read_line : cannot handle token. line="+std::to_string(line_count)+", col="+ std::to_string(col)+", token="+token+", error="+e.what();
                throw std::runtime_error(std::move(err));
            }catch(...){
                std::string err = "Unknown error in Csv_reader::read_line : cannot handle token. line="+std::to_string(line_count)+", col="+ std::to_string(col)+", token="+token;
                throw std::runtime_error(std::move(err));
            }
        }
    };

    tokenize(line_str,sep,fn_col);
    at_line(line_count);

    return true;

}

void csv::Csv_reader::reset(){
    line_count=0;
    fn_vector.resize(0);
}



size_t csv::Csv_reader::read(std::istream &in, const std::string &name_){
    reset();
    name=name_;

    read_header(in);

    while(read_line(in)){};
    return line_count;
}

size_t csv::Csv_reader::read(const std::filesystem::path &p){
    std::ifstream in( p );
    if(!in){
        throw std::runtime_error("Error in Csv_reader::read_file, cannot open file. path="+p.generic_string() );
    }
    return read(in,p.generic_string());
}

