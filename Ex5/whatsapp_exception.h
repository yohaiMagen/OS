//
// Created by yohai on 6/25/17.
//

#ifndef EX5_WHATSAPP_EXCEPTION_H
#define EX5_WHATSAPP_EXCEPTION_H
// -----------------------------------------------includes------------------------------------------------------------
#include <system_error>
// -----------------------------------------------defines------------------------------------------------------------
#define SYSTEM_ERR(name) std::string("ERROR: ") + std::string(name) +\
        std::string(" ") +std::to_string(errno) + std::string(".\n")

// --------------------------------------------deceleration------------------------------------------------------------
/**
 * an exeption class for the whatsapp app. thrown when system failer occur
 */
class whatsapp_exeption: public std::exception
{
    //the err msg for the user
    std::string _err;
public:
    /**
     * whatsapp_exception constructor
     * @param err the err msg
     */
    whatsapp_exeption(std::string err):_err(err){}
    /**
     *
     * @return the err msg
     */
    virtual const char* what() const throw(){ return _err.c_str();}

};

#endif //EX5_WHATSAPP_EXCEPTION_H
