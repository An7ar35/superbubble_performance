#include "SB_Driver.h"

/**
 * Constructor
 * @param writer FileWriter used to save runtime benchmarks
 */
sbp::algo::SB_Driver::SB_Driver( eadlib::io::FileWriter &writer ) :
    _writer( writer )
{}

/**
 * Destructor
 */
sbp::algo::SB_Driver::~SB_Driver() {}

/**
 * Runs the O(n + m) time superbubble identification algorithm
 * @param graph   Graph to detect superbubbles on
 * @param sb_list List to store SuperBubbles found into
 */
void sbp::algo::SB_Driver::runLinear( const eadlib::WeightedGraph<size_t> &graph,
                                      std::list<container::SuperBubble> &sb_list ) {
    auto timer  = sbp::chrono::Timer();
    timer.mark( "start" );
    auto sb = SB_Linear( graph );
    if( !sb.run( sb_list ) ) {
        //TODO error control
    }
    timer.mark( "end" );
    auto writer = eadlib::io::FileWriter( "benchmarks.txt" );
    timer.outputToFile( writer, "Linear SuperBubble Algorithm" );
}

/**
 * Runs the O(m Log m) time superbubble identification algorithm
 * @param graph Graph to detect superbubble on
 * @param sb_list List to store SuperBubbles into
 */
void sbp::algo::SB_Driver::runQLinear( const eadlib::WeightedGraph<size_t> &graph,
                                       std::list<sbp::algo::container::SuperBubble> &sb_list ) {
    auto timer  = sbp::chrono::Timer();
    timer.mark( "start" );
    auto sb = SB_QLinear( graph );
    if( !sb.run( sb_list ) ) {
        //TODO error control
    }
    timer.mark( "end" );
    auto writer = eadlib::io::FileWriter( "benchmarks.txt" );
    timer.outputToFile( writer, "Quasi-Linear SuperBubble Algorithm" );
}
