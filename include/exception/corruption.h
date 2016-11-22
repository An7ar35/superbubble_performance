#ifndef EADLIB_CORRUPTION_H
#define EADLIB_CORRUPTION_H

#include <string>
#include <exception>

namespace eadlib {
    namespace exception {
        class corruption : public std::exception {
          public:
            corruption( const std::string &msg ) : m_msg( msg ) {}

            virtual const char *what() const throw() {
                return m_msg.c_str();
            }

          private:
            std::string m_msg;
        };
    }
}
#endif //EADLIB_CORRUPTION_H
