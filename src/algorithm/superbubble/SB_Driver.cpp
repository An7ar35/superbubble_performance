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
    auto sb = SB_Linear( graph );
    //TODO benchmark
    auto n = sb.run( sb_list );
}
