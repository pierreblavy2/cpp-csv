/*
Copyright (C) 2024 Pierre BLAVY

This program (cpp-csv) is free software; you can redistribute it and/or
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


#include "Csv_writer.hpp"
#include <fstream>



namespace csv{

Csv_writer::Csv_writer(char sep_, char endl_):sep(sep_),endl(endl_){};


Csv_writer::~Csv_writer(){ if(own_out){ try{delete out;}catch(...){} } }

void Csv_writer::set_write(std::ostream &out_, const std::string &name_){
    auto finally=[&,this](){
        line_count=0;
        own_out = false;
        out = &out_;
        name=name_;
        check();
    };

    if(own_out){
        try{
            delete out;
        }catch(...){
            finally();
            throw;
        }
    }
    finally();
}

void Csv_writer::set_write(const std::filesystem::path &p){

    auto finally=[&,this](){
        line_count=0;
        name=p.generic_string();
        own_out = true;
        out = new std::ofstream(p);
        if(out==nullptr){throw std::runtime_error("Error in Csv_writer : cannot create new ofstream, name="+name);}
        check();
    };

    if(own_out){
        try{
            delete out;
            out=nullptr; //(1)
        }catch(...){
            out=nullptr;//(1)
            finally();
            throw;
        }
        //(1) important, as new std::ofstream(p); may throw, we don't want to delete the old out value twice
    }
    finally();

}




size_t Csv_writer::get_column(const std::string &col_name)const{
    auto f = colname_to_index.find(col_name);
    if(f==colname_to_index.end()){
        throw std::runtime_error("Error in Csv_writer : the column doesn't exists, colname="+col_name+", name="+name);
    }
    return f->second;
}


void Csv_writer::write_header(){
    auto b = std::cbegin(header_v);
    auto e = std::cend  (header_v);

    if(b != e){*out << *(*b); ++b;}
    while(b!=e){
        *out<< sep<< *(*b);
        ++b;
    }
    *out << endl;
    ++line_count;
    col_count=0;
}

void Csv_writer::write_endl(){
    //write line, set all line_v value to "";
    auto b = std::begin(line_v);
    auto e = std::end  (line_v);

    if(b != e){*out << (*b); *b=""; ++b;}
    while(b!=e){
        *out<< sep<< (*b);
        *b="";
        ++b;
    }

    *out << endl;
    ++line_count;
    col_count=0;
}







void Csv_writer::close(){
    if(own_out){
        try{
            delete out;
            out=nullptr;
            own_out=false;
        }catch(...){
            out=nullptr;
            own_out=false;
            throw;
        }
    }


    name="<closed "+name+">";
    std::fill(line_v.begin(), line_v.end(), "");
    line_count=0;
    col_count=0;
}

void Csv_writer::reset(){
    close();
    header_v.clear();
    line_v.clear();

    colname_to_index.clear();
}


void Csv_writer::check(){
    if(!*out){throw std::runtime_error("Error in Csv_writer : cannot write, name="+name);}
};



}//end csv
