# cpp-csv
A simple csv parser

# Read a csv file
Input csv example
```
city,habs
New York city,8.8 M
London,9.8 M
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

# Write a csv file

```c++
csv::Csv_writer w;
w.set_write("out.csv"); //note, you can also pass a ostream& and a name

//define header
//i_city and i_habs will contain the column indexes
size_t i_city = w.add_column("city");
size_t i_habs = w.add_column("habs");

//write header (call this once)
w.write_header();

//write column content by name (slow)
w.write_token("city","New York");
w.write_token("habs","3.80 M");
w.write_endl();

//write column by column index (fast)
//NOTE if you need to find column index, call i_habs=w.get_column("column_name");
w.write_token(i_habs,"2.10 M");
w.write_token(i_city,"Paris");
w.write_endl();

//write tokens, knowing their column indexes. (faster)
w.write_tokens_at(0, "Rome");
w.write_tokens_at(1, "2.8 M");
w.write_endl();

w.write_tokens_at(0, "Toulouse", "471k");  //equivalent to : w.write_tokens_at(0, "Toulouse"); w.write_tokens_at(1, "471k");
w.write_endl();

//write tokens. They must arrive in the correct order. (faster)
w.write_tokens("LA","3.82 M");
w.write_endl();

w.write_tokens("Brest");
w.write_tokens("139 k");
w.write_endl();

//write a full line (fastest);
//you need everything, in the right order.
w.write_line("Tunis","599 k");

// Don't forget to reset or close if w owned the ofstream and if you want to flush output before destruction.
 w.close();
```






