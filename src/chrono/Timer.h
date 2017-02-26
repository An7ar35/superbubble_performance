/**
    @class          sbp::chrono::Timer
    @brief          Multi-mark Timer
    @author         E. A. Davison
    @copyright      E. A. Davison 2017
    @license        GNUv2 Public License
    @dependencies   eadlib::io::FileWriter, eadlib::logger::Logger
**/
#ifndef SUPERBUBBLE_PERFORMANCE_TIMER_H
#define SUPERBUBBLE_PERFORMANCE_TIMER_H

#include <list>
#include <eadlib/io/FileWriter.h>
#include <eadlib/logger/Logger.h>

namespace sbp {
    namespace chrono {
        class Timer {
          public:
            Timer();
            ~Timer();
            void mark( const std::string &name );
            void reset();
            bool outputToFile( eadlib::io::FileWriter &writer, const std::string &timer_name );

          private:
            struct TimerMark {
                TimerMark( const std::string &name, const std::clock_t &time ) :
                    _name( name ),
                    _mark( time )
                {};
                ~TimerMark() {};
                std::string _name;
                std::clock_t _mark;
            };
            std::list<TimerMark> _timer_marks;
        };
    }
}


#endif //SUPERBUBBLE_PERFORMANCE_TIMER_H
