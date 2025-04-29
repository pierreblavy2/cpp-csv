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

#ifndef CSV_READER_PIERRE_HPP
#define CSV_READER_PIERRE_HPP

#include <unordered_map>
#include <vector>

#include <string>
#include <functional>
#include <istream>
#include <filesystem>




namespace csv{

//USAGE :
//Csv_reader r('\t');
//
//--- define parser --
//std::pair<std::string,std::string> p;
//std::vector< std::pair<std::string,std::string> > v;
//r.add_column("col1",[&](size_t line, std::string&& token){p.first =token;});
//r.add_column("col2",[&](size_t line, std::string&& token){p.second=token;});
//r.at_line=[&](size_t){v.push_back(p);}
//
//Optional : simplify column names
//r.at_header = [](std::string&s){csv::trim(s);}
//
//Optional : simplify tokens
//r.at_token = [](std::string&s){csv::trim(s);}
//
//--- run parser ---
//r.read("something.tsv");




class Csv_reader{
    public:
    typedef std::function<void(size_t line, std::string&&)> Fn_column;
    typedef std::function<void(size_t line)> Fn_line; //line 0 is header, line 1 is first data line

    explicit Csv_reader(char sep_='\t' ,char  endl_='\n'):sep(sep_),endl(endl_){}

    template<typename Fn>
    void add_column(std::string col_name, Fn&& fn){
        colname_to_fn[col_name] = std::forward<Fn>(fn);
    }

    size_t read(std::istream &in, const std::string &name);
    size_t read(const std::filesystem::path &p);

    Fn_line at_line=[](size_t){};
      //called at the end of each line

    std::function<void(std::string&)> at_header = [](std::string&){};
      //called after reading a header => this is the place to simplify header names
      //for example by removing quotes, triming them ...

    std::function<void(std::string&)> at_token = [](std::string&){};
        //called after reading a token on a defined column => this is the place to simplify tokens
        //for example by removing quotes, triming them ...

    char sep;
    char endl;

    private:
    std::string name; //used to produce clear error messages
    size_t line_count=0;
    std::unordered_map<std::string,Fn_column> colname_to_fn;
    std::vector<Fn_column*> fn_vector;

    //details : read file line by line
    void read_header(std::istream &in);
    bool read_line  (std::istream &in);
    void reset();

};



}

#endif
