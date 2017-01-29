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
    auto found_SCCs = Tarjan( _graph ).findSCCs();

    //Partition graph into sub-graphs
    auto sub_graphs = PartitionGraph().partitionSCCs( _graph, *found_SCCs, "SubGraph" );
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    //Convert SubGraphs into DAG
    auto dag_packages = sbp::algo::GraphToDAG().convertToDAG( *sub_graphs, "DAG" );

    return false;
}
