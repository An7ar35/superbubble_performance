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
size_t sbp::algo::SB_Linear::run() {
    //TODO
    auto tarjan = std::make_unique<Tarjan>( Tarjan( _graph ) );
    for( auto list : *tarjan ) {
        for( auto e : list ) {
            std::cout << e << ",";
        }
        std::cout << std::endl;
    }

    return 0;
}

