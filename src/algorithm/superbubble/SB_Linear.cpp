#include "SB_Linear.h"

/**
 * Constructor
 * @param graph Graph on which to detect superbubbles
 */
sbp::algo::SB_Linear::SB_Linear( const eadlib::WeightedGraph<size_t> &graph ) :
    _graph( graph )
{}

/**
 * Destructor
 */
sbp::algo::SB_Linear::~SB_Linear() {}

/**
 * Run the Linear SuperBubble identification algorithm
 * @param superbubble_list SuperBubble list container to fill
 * @return Success
 */
bool sbp::algo::SB_Linear::run( std::list<container::SuperBubble> &superbubble_list ) {
    //Find SCCs
    auto found_SCCs = Tarjan( _graph ).findSCCs();

    //Partition graph into sub-graphs
    auto sub_graphs = PartitionGraph().partitionSCCs( _graph, *found_SCCs, "SubGraph" );
    found_SCCs.reset(); //no longer needed so early destruction to free up memory



    //TODO
    return 0;
}

