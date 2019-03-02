#pragma once
#include <optional>
#include <vector>
#include "sqlite_modern_cpp.h"
#include "methods_to_support.h"
#include <fstream>

struct casting_role //роль и актер
{

    std::string actor;
    std::string role;

};

struct movie//структура кинофильм
{
 unsigned int  id;//id
 std::string  title;//название фильма
 int year;//год
 int lenght;//продолжительность
 std::vector<casting_role> cast;//список(актер,роль)
 std::vector<std::string> directors;//список рижессеров
 std::vector<std::string> writers;//список сценаристов

};

struct media//для хранения медиа-данных фильма(картинки,музыка,видео)
{

    unsigned int id;// for rowid
    unsigned int movie_id;//id
    std::string name;//название медиа
    std::optional<std::string> text;//описание
    std::vector<char> blob;//данные


};
using media_list =std::vector<media>;
using movie_list=std::vector<movie>;

///////////////////////////////////////////////////function at work for structure movie/////////////////////////////////////////
void print_movie(const movie& m)//печать базы
{

 std::cout<<"[" <<m.id<< "] ";
 std::cout << m.title  <<"(" <<m.year <<") ";
 std::cout << m.lenght<< "min "<<std::endl;

 std::cout<< "direct by :";
  for(const auto& dir:m.directors)
  {
     std::cout<<dir<<" ,";

  }
   std::cout<<std::endl;
    std::cout<< "written by :";
  for(const auto& wr:m.writers)
  {
      std::cout<<wr<<" ,";

  }
    std::cout<<std::endl;
    std::cout<< "cast by :"<<std::endl;
  for(const auto& ca:m.cast)
  {
      std::cout<<ca.actor<< "(" <<ca.role<< "),";
      std::cout<<std::endl;

  }

}

 std::vector<casting_role> get_cast(const sqlite3_int64 movie_id,//возвращает вектор(актер и его роль)
                              sqlite::database& db)
{
    std::vector<casting_role>result;
    db<<R"(select p.name , c.role from casting as c
         join persons as p on c.personid = p.rowid where c.movieid = ?;)"
           <<movie_id
           >>[&result](const std::string name, std::string role)
           {
               result.emplace_back(casting_role{name,role});
           };

    return result;

}

std::vector<std::string> get_directors(const sqlite3_int64  movie_id,//возвращает режиссера
                             sqlite::database & db)
{

    std::vector<std::string>result;
    db<< R"(select p.name from  directors as d
        join persons as p on d personid = p.rowid where d.movieid = ?;)"
           <<movie_id
           >>[&result](const std::string name)
           {
               result.emplace_back(name);
           };

return result;

}

std:: vector<std::string> get_writers(sqlite3_int64 const move_id,
                           sqlite::database& db)//возвращает сценариста
{
std::vector<std::string> result;
db<<R"(select p.name from writers as w
    join persons as p on w.personid = p.rowid where w.movieid =?;)"
<< move_id
>>[&result](std::string const name)
{
    result.emplace_back(name);
};
  return result;

 }

movie_list get_movis(sqlite::database& db)//возвращает список фильмов из базы
{
    movie_list movies;
   db<<R"(select rowid,*  from movies;)"
    >>[&movies,&db]( sqlite3_int64 const rowid,
                    const std::string& title,
                    const int year,const int lenght)
                    {
                        movies.emplace_back(movie{
                                  static_cast<unsigned int>(rowid),
                                            title,
                                            year,
                                        static_cast<unsigned int> (lenght),

                                            get_cast(rowid,db),



                                           get_writers(rowid,db),
                                              get_writers(rowid,db)
                                           // get_directors(rowid,db)
                                            });
                    };

return movies;
}

/////////////////////////////////////////////////////add information in our base///////////////////////////////////////////////////
movie read_movie()//метод позволяет пользователю ввести данные(название фильма,год,продолжительность,
//актеры их роли,режиссеры и сценаристы
{
    movie m;
    std::cout<< "Please enter movie"<<"\n";
    std::cout <<"Title: ";
    getline(std::cin,m.title);
    std::cout<<"Year: ";std::cin>>m.year;
    std::cout<<"Lenght: ";std::cin>>m.lenght;
    std::cin.ignore();
    std::string directors;
    std::cout<< "Directors: ";
    getline(std::cin,directors);
    m.directors=split(directors,',');
    std::string writers;
    std::cout<< "Writers: ";
    getline(std::cin,writers);
    m.writers=split(writers,',');
    std::string cast;
    std::cout<< "Cast: ";
    getline(std::cin,cast);
    auto roles= split(cast,',');
    for(const auto & r:roles)
    {

        auto pos =r.find_first_of('=');
        casting_role cr;
        cr.actor=r.substr(0,pos);
        cr.role=r.substr(pos+1,r.size()-pos-1);
        m.cast.push_back(cr);

    }
    return m;
}

sqlite_int64 get_person_id(const std::string& name, sqlite::database& db)//возвращает id персоны
{
    sqlite_int64 id=0;
    db<<"select rowid from persons where name=?;"
    <<name
    >>[&id](const sqlite_int64 rowid ){id=rowid;};
    return id;
}

sqlite_int64 insert_person(std::string_view name,sqlite::database& db)//вставляет  в таблицу persons  базs данных
{

    db<<"insert into persons values(?);"
    <<name.data();
    return db.last_insert_rowid();
}
void insert_directors(sqlite_int64 const move_id,
                      const std::vector<std::string>& directors,
                      sqlite::database& db)
{
    for(const auto& director: directors)
    {
        auto id= get_person_id(director,db);
        if(id==0)id=insert_person(director,db);
        db<<"insert into directors values(?,?);"
        <<move_id
        <<id;
    }

}
void insert_writers(sqlite_int64 const move_id, //вставляет сценариста в таблицу writers базы данных
                      const std::vector<std::string>& writers,
                      sqlite::database& db)
{
    for(const auto& writer: writers)
    {
        auto id= get_person_id(writer,db);
        if(id==0)id=insert_person(writer,db);
        db<<"insert into writers values(?,?);"
        <<move_id
        <<id;
    }

}

void insert_cast(sqlite_int64 const move_id,//вставляет актера и его роль в таблицу casting базы данных
                      const std::vector<casting_role>& cast,
                      sqlite::database& db)
{
    for(const auto& cr: cast)
    {
        auto id= get_person_id(cr.actor,db);
        if(id==0)id=insert_person(cr.actor,db);
        db<<"insert into casting values(?,?,?);"
        <<move_id
        <<id
        <<cr.role;
    }

}

void insert_movie(movie& m,sqlite::database& db)//вызывает предыдущие методы,с послейдующей вставкой в таблицу movies базы данных
{
    try{

      db<<"begin;";

      db<<"insert into movies values(?,?,?);"
      <<m.title
      <<m.year
      <<m.lenght;

      auto movieid=db.last_insert_rowid();
      insert_directors(movieid,m.directors,db);
      insert_writers(movieid,m.writers,db);
      insert_cast(movieid,m.cast,db);
      m.id=static_cast<unsigned int>(movieid);
      db<<"commit;";
    }
    catch(const std::exception &)
    {
        db<<"rollback;";
    }

}
////////////////////////////////methods for at work with media-file our films///////////////////////////////////////////////////////

inline bool start_with(std::string_view text,std::string_view part)//проверяет что заданная строка начинается с указанной подстроки
{
    return text.find(part)==0;
}


std::vector<char> load_images(std::string_view filepath)//вспомагательная функция(парсит данные из файла в вектор char)
{

    std::vector<char> data;
   std::ifstream file(filepath.data(),std::ios::binary | std::ios::ate);
    if(file.is_open())
    {
        auto size=file.tellg();
        file.seekg(0,std::ios::beg);
        data.resize(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(data.data()),size);

    }
    return data;

}

bool add_media(const sqlite_int64 movieid,
              std::string_view name,
               std::string_view description,std::vector<char>content,sqlite::database& db)//добавляет данные в базу

{
 try
 {

    db<<"insert into media values(?,?,?,?)"
      <<movieid
      <<name.data()
      <<description.data()
      <<content;
      return true;
 }
   catch(...){return false;}

}



void run_add(std::string_view line,sqlite::database& db)//добавляет медиафайл для фильма
//извлекает из ввода id фильма,путь к файлу и его описание и возвращает в виде списка элементов
// разделенных запятыми(add <movieid>,<path>,<description>)
{
     auto parts=split(trim(line.substr(4)),',');
      if(parts.size()==3)
  {

      std::string ln{line};
       std:: vector<std::string> ads=split_to(ln);
        auto movieid = stoi(parts[0]);
        std:: string_view path=ads[1].data();
        auto desc=ads[2];
        auto name=ads[1].substr(0,ads[1].find('.'));
        auto content= load_images(path);
        auto succes= add_media(movieid,name,desc,content,db);
        if(succes)
        {

           std::cout<<"added!"<<std::endl;

        }
        else{
           std::cout<<"failed!"<<std::endl;
        }

  }else
 std::cout<<"input error"<<std::endl;

}
bool delete_media (const sqlite_int64 mediaid,sqlite::database& db)//удаляет данные медиа данные из базы
{
 try{

 db<<"delete from media where rowid=?;"
 <<mediaid;
   return true;
 }
 catch(...){return false;}

}
void run_del(std::string_view line, sqlite::database& db)
{

    auto mediaid=stoi(trim(line.substr(4)));
    if (mediaid>0)
    {
        auto succes=delete_media(mediaid,db);
        if(succes){std::cout<<"deleted!"<<std::endl;;}
        else
            std::cout<<"failed!"<<std::endl;
    }else
    std::cout<<"input error!"<<std::endl;

}
media_list get_media(const sqlite_int64 movieid,sqlite::database& db)//метод извлекает медиа данные из бызы
{
media_list list_;
  db<<"select rowid,* from media where movieid=?;"
    <<movieid
    >>[&list_](const sqlite_int64 rowid,
               const sqlite_int64 movieid,
               const std::string& name,
               const std::optional<std::string> text,const std::vector<char> blob)
               {

                   list_.emplace_back(media{
                                      static_cast<unsigned int>(rowid),
                                      static_cast<unsigned int>(movieid),
                                      name,
                                      text,
                                      blob
                                      });

               };

  return list_;

}

void run_list(std::string_view line, sqlite::database& db)//извлекает id фильма из ввода пользователя
{

    auto movieid = std::stoi(trim(line.substr(5)));
    if(movieid > 0)
    {
        auto list_ =get_media(movieid,db);//получить список медифайлов этого фильма
        if(list_.empty())
        {
            std::cout<<"empty"<<std::endl;
        }
        else
        {
          for(const auto& m: list_)
          {
              std::cout<<m.id<<" | "
              <<m.movie_id <<" | "
              <<m.name <<" | "
              <<m.text.value_or("(null)")<<" | "
              <<m.blob.size()<<" bytes"<<std::endl;

           }

        }

    }else
    std::cout<<"input error"<<std::endl;

}
movie_list get_movies(std::string_view title,sqlite::database& db)//возвращает идентификатор фильма
{

 movie_list movies;
    db<<R"(select rowid, * from movies where title =?;)"
     <<title.data()
     >>[&movies,& db](const sqlite_int64 rowid,
                      const std::string& title,
                      const int year, const int lenght)
                      {
                          movies.emplace_back(movie{
                                              static_cast<unsigned int>(rowid),
                                              title,
                                              year,
                                              static_cast<unsigned int> (lenght),
                                              {},{},{}


                                              });
                      };

return movies;
}
void run_find(std::string_view line,sqlite::database& db)//извлекает название фильма и ввода пользователя
{

    auto title = trim(line.substr(5));
    auto movies=get_movies(title,db);//вызываем что бы получить список фильмов с этим названием.
    if(movies.empty())
     std::cout<<"empty"<<std::endl;
    else
    {
       for(const auto m:movies)    //выводим в консоль
        {

            std::cout<<m.id<<" | "
            <<m.title<<" | "
            <<m.year<<" | "
            <<m.lenght<<"min"
            <<std::endl;
        }

    }

}






void print_comands()
{

std::cout
          <<"*************************************************************************\n";
std::cout <<"print                                   prints database contents        *\n"
          <<"insert                                  add database contents           *\n"
          <<"Find title                              Find a movie ID                 *\n"
          <<"List movieid                            lists the images of a movie     *\n"
          <<"add <movieid>, <path>, <description>    adds a new images               *\n"
          <<"del images                              delete an images                *\n"
          <<"help                                    show available commands         *\n"
          <<"exit                                    exists the application          *\n"
          <<"*************************************************************************"<<std::endl;
}







