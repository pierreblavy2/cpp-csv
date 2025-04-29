#include <csv/Csv_reader.hpp>
#include <csv/Csv_writer.hpp>
#include <iostream>

#include <csv/tools/trim.hpp>

int main(int argc, char *argv[])
{


    //=== WRITE things ===
    csv::Csv_writer w;
    auto i_city = w.add_column("city");
    auto i_habs = w.add_column("habs");

    auto write_fn = [&](csv::Csv_writer &w){
        w.write_header();

        //write column content by name (slow)
        w.write_token("city","New York");
        w.write_token("habs","3.80 M");
        w.write_endl();

        //write column by column index (fast)
        //NOTE if you need to find column index, call i_habs=w.get_column();
        w.write_token(i_habs,"2.10 M");
        w.write_token(i_city,"Paris");
        w.write_endl();

        //write tokens. They must arrive in the correct order. (faster)
        w.write_tokens("LA","3.82 M");
        w.write_endl();

        w.write_tokens("Brest");
        w.write_tokens("139 k");
        w.write_endl();

        //write tokens, knowing their column indexes. (faster)

        w.write_tokens_at(0, "Toulouse", "471k");

        w.write_tokens_at(1, "2.8 M");
        w.write_tokens_at(0, "Rome");


        //write a full line (fastest);
        //you need everything, in the right order.
        w.write_line("Tunis","599 k");

        // Don't forget to reset or close if w owned the ofstream and if you want to flush output before destruction.
        w.close();
    };


    //open
    w.set_write("test.csv");                          //w will own a std::ofstream, that writes in "test.csv" and thats named "test.csv" (1)
    write_fn(w);

    w.set_write(std::cout, "Standard output");    //w will reference without owning an ostream&  thats named "Standard output" (1)
    write_fn(w);

    //(1) names are only used to provide relevant error messages.






    std::cout << "\n===\n\n";


    //=== READ things ===

    csv::Csv_reader r;

    std::string habs = "";
    std::string city = "";
    r.add_column("habs",[&](size_t, const std::string &s){habs=s;});
    r.add_column("city",[&](size_t, const std::string &s){city=s;});

    //optional
    r.at_header=[](std::string &s){csv::trim(s);}; //trim column names
    r.at_token =[](std::string &s){csv::trim(s);}; //trim all tokens

    r.at_line=[&](size_t line){std::cout << line << "\t" << habs <<"\t"<< city<<"\n"; habs="", city="";};
    r.read("test.csv");
}
