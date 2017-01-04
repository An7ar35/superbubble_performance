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
    //TODO
    auto strongly_connected_components = Tarjan( _graph ).findSCCs();
    auto singletonSCC_count            = Tarjan::concatenateSingletonSCCs( *strongly_connected_components );
    auto subGraphs_count               = strongly_connected_components->size();
    auto subGraphs                     = std::make_unique<std::vector<eadlib::Graph<size_t>>>();
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

