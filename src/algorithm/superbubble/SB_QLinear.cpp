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
    auto found_SCCs         = Tarjan( _graph ).findSCCs();
    auto singletonSCC_count = Tarjan::concatenateSingletonSCCs( *found_SCCs );

    //Partition graph into sub-graphs
    auto sub_graphs = algo::PartitionGraph();
    size_t sg_count { 0 };
    auto it = found_SCCs->begin();
    if( it != found_SCCs->end() ) { //Singleton SCCs
        sub_graphs.partitionSingletonSCCs( _graph, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    for( auto it = std::next( it ); it != found_SCCs->end(); ++it) { //All the other SCCs
        sub_graphs.partitionSCC( _graph, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    found_SCCs.reset(); //no longer needed so early destruction to free up memory




    return false;
}
