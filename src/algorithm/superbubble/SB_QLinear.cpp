#include "SB_QLinear.h"
#include "../../graph/SubGraph.h"

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
    //Find SCCs
    auto strongly_connected_components = Tarjan( _graph ).findSCCs();
    auto singleton_count = Tarjan::concatenateSingletonSCCs( *strongly_connected_components );

    //Partition graph into sub-graphs
    std::list<std::unique_ptr<graph::SubGraph>> sub_graphs;
    size_t sg_count { 0 };
    for( auto it = strongly_connected_components->begin(); it != strongly_connected_components->end(); ++it) {
        sub_graphs.emplace_back( std::make_unique<graph::SubGraph>( _graph, *it, std::string( "SubGraph" + sg_count ) ) );
        sg_count++;
    }




    return false;
}
