#include <string>
#include <dirent.h>
#include <iostream>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"


class DirName:public k1Base
{

    std::string _dir_name;
public:
    DirName(const std::string &str):_dir_name(str){}
    std::string get_dir_name() const
    {
        return _dir_name;
    }
    bool operator<(const k1Base &other) const
    {
        return _dir_name.compare(dynamic_cast<const DirName &>(other)._dir_name) < 0;
    }
    ~DirName(){}
};
 class SubFileName: public v1Base
 {
 public:
     std::string _substring;
     SubFileName(std::string &str): _substring(str){}
 };

class FileName:public k2Base, public k3Base
{

    std::string _file_name;
public:
    FileName(const std::string &str):_file_name(str){}
    FileName(const FileName &other):_file_name(other._file_name){}

    std::string get_file_name() const
    {
        return _file_name;
    }

    bool operator<(const k2Base &other) const
    {
        return _file_name.compare(dynamic_cast<const FileName &>(other)._file_name) < 0;
    }

    bool operator<(const k3Base &other) const
    {
        return _file_name.compare(dynamic_cast<const FileName &>(other)._file_name) < 0;
    }
    ~FileName(){}
};

class ContainsSubstring: public v2Base
{
public:
    int _contains;
    ContainsSubstring():_contains(1){}
};

class RepeatFileName: public v3Base
{
public:
    int _num_rep;
    RepeatFileName(const int & num_repeat): _num_rep(num_repeat){}
    RepeatFileName& operator++()
    {
        _num_rep++;
        return *this;
    }
    operator int() const
    {
        return _num_rep;
    }
};
class SearchFileName: public MapReduceBase
{

public:
    void Map(const k1Base *const key, const v1Base *const val) const
    {
        std::string path = dynamic_cast<const DirName *>(key)->get_dir_name();
        std::string sub_str = dynamic_cast<const SubFileName *>(val)->_substring;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path.c_str())) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                std::string file_name(ent->d_name);
                if(file_name.find(sub_str) != std::string::npos)
                {
                    Emit2(new FileName(file_name), new ContainsSubstring());
                }
            }
            closedir (dir);
        }
    }
    void Reduce(const k2Base *const key, const V2_VEC &vals) const
    {
        Emit3(new FileName(*dynamic_cast< const FileName*>(key)), new RepeatFileName(vals.size()));
        //TODO free key
    }
};

int main (int argc, char **argv)
{
    if(argc < 3)
    {
        //TODO print err
    }
    else
    {
        SearchFileName searcher;
        std::string str = argv[1];
        SubFileName searched = SubFileName(str);
        IN_ITEMS_VEC input;

        for (int i = 2; i < argc ; ++i)
        {
            str = argv[i];
            input.push_back(std::make_pair(new DirName(str), &searched));

        }
        OUT_ITEMS_VEC output = RunMapReduceFramework(searcher, input, 1, true);
        for (unsigned int j = 0; j < output.size() ; ++j)
        {
            for (int i = 0; i < *dynamic_cast<RepeatFileName *>(output[j].second) ; ++i)
            {
                std::cout << dynamic_cast<FileName *>(output[j].first)->get_file_name()<< " ";
//                //TODO print syntax
            }
            delete(output[j].first);
            delete(output[j].second);
        }
        // free INPUT_VEC
        for (unsigned int i = 0; i < input.size(); ++i)
        {
            delete (input[i].first);
        }
    }
}
