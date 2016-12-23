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
 * Run the detection algorithm
 * @return Number of Superbubble detected
 */
bool sbp::algo::SB_Linear::run( std::list<container::SuperBubble> &superbubble_list ) {
    //TODO
    auto strongly_connected_components = Tarjan( _graph ).findSCCs();
    auto singletonSCC_count            = concatenateSingletonSCCs( *strongly_connected_components );
    auto subGraphs_count               = strongly_connected_components->size();
    auto subGraphs                     = std::vector<std::unique_ptr<eadlib::Graph<size_t>>>();
    auto global2local_id_index         = std::make_unique<std::vector<size_t>>( _graph.nodeCount() );

    std::cout << "Found " << singletonSCC_count << " singleton SCCs." << std::endl;
    for( auto scc : *strongly_connected_components ) {
        for( auto vertex_id : scc ) {
            std::cout << vertex_id << ",";
        }
        std::cout << std::endl;
    }

    return 0;
}

/**
 * Concatenate all singletons sccs found in the graph into the first list of SCCs in the Tarjan instance
 * @param scc_list Tarjan instance
 * @return Number of singletons found
 */
size_t sbp::algo::SB_Linear::concatenateSingletonSCCs( std::list<std::list<size_t>> &scc_list ) {
    size_t count { 0 };
    auto first = scc_list.begin();
    auto next  = scc_list.begin();
    if( first != scc_list.end() && scc_list.size() > 1 ) {
        if( first->size() == 1 ) {
            next++;
            count++;
            while( next != scc_list.end() && next->size() == 1 ) {
                first->emplace_back( next->front() );
                next = scc_list.erase( next );
                count++;
            }
        }
    }
    return count;
}

