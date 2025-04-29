# cpp-csv
A simple csv parser

# Read a csv file
Input csv example
```
city,habs
New York city,8 800 000
London,9 787 426
```


```c++

#include <csv/Csv_reader.hpp>
#include <csv/tools/trim.hpp>

csv::Csv_reader r;
r.sep = ','; //set the column separator
r.endl='\n'; //set the end of line


//optional : modify read tokens
r.at_header=[](std::string &s){csv::trim(s);}; //trim column names
  //called after reading a header => this is the place to simplify header names
  //for example by removing quotes, triming them ...

r.at_token =[](std::string &s){csv::trim(s);}; //trim all tokens
  //called after reading a token on a defined column => this is the place to simplify tokens
  //for example by removing quotes, triming them ...

//required : how to handle values in each column
//attach functions to columns. These function will be called for each line and each column
//the first parameter (size_t) is the line number, the second (const std::string&) the content of the column
//if no function is attached to a column that exists in the inupt file, the code ignore this column and continue parsing

std::string habs = "";
std::string city = "";
r.add_column("habs",[&](size_t, const std::string &s){habs=s;});
r.add_column("city",[&](size_t, const std::string &s){city=s;});

//required :
r.at_line=[&](size_t line){std::cout << line << "\t" << habs <<"\t"<< city<<"\n"; habs="", city="";};
  //called at the end of each line

r.read("test.csv");

```
