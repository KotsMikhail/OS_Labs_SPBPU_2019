//
// Created by dmitrii on 11.10.2019.
//

#ifndef INOTIFY_EXCEPTION_HPP
#define INOTIFY_EXCEPTION_HPP

#include <exception>
#include <iostream>

using namespace std;

class CommonException : public std::exception
{
public:

    explicit CommonException(std::string mess):m_mess(std::move(mess)){}
    explicit CommonException(const char *mess):m_mess(mess){}

    const char * what() const throw()
    {
        return m_mess.c_str();
    }

private:

    std::string m_mess;
};

#endif //INOTIFY_EXCEPTION_HPP
