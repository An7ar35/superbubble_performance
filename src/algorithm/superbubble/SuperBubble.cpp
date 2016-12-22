#include "SuperBubble.h"

/**
 * Constructor
 * @param writer FileWriter used to save runtime benchmarks
 */
sbp::algo::SuperBubble::SuperBubble( eadlib::io::FileWriter &writer ) :
    _writer( writer )
{}

/**
 * Destructor
 */
sbp::algo::SuperBubble::~SuperBubble() {}

/**
 * Runs the O(n + m) time superbubble identification algorithm
 * @param graph Graph to detect superbubbles on
 */
void sbp::algo::SuperBubble::runLinear( const eadlib::WeightedGraph<size_t> &graph ) {
    auto sb = SB_Linear( graph );
    //TODO benchmark
    auto n = sb.run();
}
