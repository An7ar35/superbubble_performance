#ifndef SUPERBUBBLE_PERFORMANCE_TIMER_TEST_H
#define SUPERBUBBLE_PERFORMANCE_TIMER_TEST_H

#include "gtest/gtest.h"
#include "../src/chrono/Timer.h"

TEST( Timer_Tests, timer ) {
    auto writer = eadlib::io::FileWriter( "times.txt" );
    auto timer  = sbp::chrono::Timer();
    timer.mark( "start" );
    usleep( 10 );
    timer.mark( "10" );
    usleep( 40 );
    timer.mark( "50" );
    usleep( 50 );
    timer.mark( "100" );
    timer.outputToFile( writer, "Timer_Tests" );
}


#endif //SUPERBUBBLE_PERFORMANCE_TIMER_TEST_H
