#include <stdio.h>
#include <iostream>

#include "struct_for_example.h"



using namespace sqlite;
using namespace std;







/////////////////////////////////////////////////add information//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main()
{/*
try
    {
        sqlite::database db(R"(films.db)");
        //


}
catch(const sqlite::sqlite_exception&  e)
{

   cerr<<e.get_code()<<" "<<e.what()<< " " <<e.get_sql();


}
catch(exception const& e)
{

    cerr<<e.what()<<endl;
}*/

 try{

   database db(R"(films.db)");
     while(true)
     {

         string line;
         getline(cin,line);
         if(line=="help")print_comands();
         else if(line=="exit")break;
         else
         {
             if(start_with(line,"find"))
                run_find(line,db);
             else if(start_with(line,"list"))
               run_list(line,db);
             else if(start_with(line,"add"))
                run_add(line,db);
             else if(start_with(line,"del"))
                run_del(line,db);
             else if(start_with(line,"print"))
             {
                 auto movies= get_movis(db);
                 for(const auto& m:movies)
               {
                    print_movie(m);
               }

             }
             else if(start_with(line,"insert"))
             {
                  auto movie=read_movie();
                  insert_movie(movie,db);
                  cout<<"data successfully added"<<endl;

             }
             else
                std::cout<<"unknown command" <<endl;

        }

     }

 }
 catch(sqlite::sqlite_exception const& e)
 {

     cerr<< e.get_code()<<": "<<e.what()<< "during " <<e.get_sql()<<endl;

 }
 catch(exception const& e){cerr<<e.what()<<endl;}

    return 0;
}





