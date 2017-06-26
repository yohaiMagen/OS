//
// Created by yohai on 6/25/17.
//

#ifndef EX5_WHATSAPP_EXCEPTION_H
#define EX5_WHATSAPP_EXCEPTION_H

#include <system_error>
#define SYSTEM_ERR(name) std::string("ERROR: ") + std::string(name) +\
        std::string(" ") +std::to_string(errno) + std::string(".\n")

class whatsapp_exeption: public std::exception
{
    std::string _err;
public:
    whatsapp_exeption(std::string err):_err(err){}
    virtual const char* what() const throw(){ return _err.c_str();}

};

#endif //EX5_WHATSAPP_EXCEPTION_H
