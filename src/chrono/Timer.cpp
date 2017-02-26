#include "Timer.h"

/**
 * Constructor
 */
sbp::chrono::Timer::Timer() {}

/**
 * Destructor
 */
sbp::chrono::Timer::~Timer() {}

/**
 * Adds a timer mark
 */
void sbp::chrono::Timer::mark( const std::string &name ) {
    _timer_marks.emplace_back( name, std::clock() );
}

/**
 * Resets timer marks (deletes them all)
 */
void sbp::chrono::Timer::reset() {
    _timer_marks.clear();
}

/**
 * Exports all timer marks
 * @param writer     FileWriter to use
 * @param timer_name Name for the set of timer marks
 * @return Success
 */
bool sbp::chrono::Timer::outputToFile( eadlib::io::FileWriter &writer, const std::string &timer_name ) {
    if( !writer.isOpen() && !writer.open() ) {
        LOG_ERROR( "[sbp::chrono::Timer::export( ", writer.getFileName(), ", ", timer_name, " )] Could not open file." );
        return false;
    }

    if( _timer_marks.size() > 1 ) {
        auto first = _timer_marks.begin();
        writer.write( std::string( "|= " + timer_name + " =|\n" ) );
        for( auto it = first; it != _timer_marks.end(); ++it ) {
            std::ostringstream oss;
            oss << "> " << it->_name.c_str() << ": "
                << ( it->_mark - first->_mark ) / ( double ) ( CLOCKS_PER_SEC / 1000 )
                << " ms" << std::endl;
            writer.write( oss.str() );
        }
    } else {
        LOG_ERROR( "sbp::chrono::Timer::export( ", writer.getFileName(), ", ", timer_name, " )] Not enough time marks for export." );
        return false;
    }
}
