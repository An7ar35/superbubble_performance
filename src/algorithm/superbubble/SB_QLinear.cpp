#include "SB_QLinear.h"

/**
 * Constructor
 * @param graph Graph on which to detect superbubbles
 */
sbp::algo::SB_QLinear::SB_QLinear( const eadlib::WeightedGraph<size_t> &graph ) :
    _graph( graph )
{}

/**
 * Destructor
 */
sbp::algo::SB_QLinear::~SB_QLinear() {}

/**
 * Runs the Quasi-Linear SuperBubble identification algorithm
 * @param superbubble_list SuperBubble list container to fill
 * @return Success
 */
bool sbp::algo::SB_QLinear::run( std::list<sbp::algo::container::SuperBubble> &superbubble_list ) {
    auto strongly_connected_components = Tarjan( _graph ).findSCCs();
    Tarjan::concatenateSingletonSCCs( *strongly_connected_components );





    return false;
}