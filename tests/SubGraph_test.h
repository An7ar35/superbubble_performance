#ifndef SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H
#define SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H

#include "gtest/gtest.h"
#include "../src/graph/SubGraph.h"
#include "../src/graph/SubGraph.cpp"

TEST( SubGraph_Tests, Constructor ) {
    auto g = eadlib::WeightedGraph<size_t>( "SCC2_concatenate_test" );
    g.createDirectedEdge_fast( 0, 1 );
    g.createDirectedEdge_fast( 0, 5 );
    g.createDirectedEdge_fast( 1, 2 );
    g.createDirectedEdge_fast( 1, 6 );
    g.createDirectedEdge_fast( 2, 3 );
    g.createDirectedEdge_fast( 2, 4 );
    g.createDirectedEdge_fast( 3, 4 );
    g.createDirectedEdge_fast( 4, 5 );
    g.createDirectedEdge_fast( 4, 1 );
    g.createDirectedEdge_fast( 5, 6 );
    g.createDirectedEdge_fast( 6, 7 );
    //SCCs
    auto strongly_connected_components = sbp::algo::Tarjan( g ).findSCCs();
    auto singleton_count = sbp::algo::Tarjan::concatenateSingletonSCCs( *strongly_connected_components );
    //Partitioning
    std::list<sbp::graph::SubGraph> sub_graphs;
//    auto it = strongly_connected_components->begin();
//    if( singleton_count > 0 && it != strongly_connected_components->end() ) { //Singleton SCCs sub-graph
//        sub_graphs.emplace_back( sbp::graph::SubGraph( g, *it ) );
//        it = std::next( it );
//    }
//    size_t sg_count { 1 };
//    for( ; it != strongly_connected_components->end(); ++it) {
//        sub_graphs.emplace_back( sbp::graph::SubGraph( g, *it, std::string( "sg" + sg_count ) ) );
//        sg_count++;
//    }

    size_t sg_count { 0 };
    for( auto it = strongly_connected_components->begin(); it != strongly_connected_components->end(); ++it) {
        sub_graphs.emplace_back( sbp::graph::SubGraph( g, *it, std::string( "sg" + sg_count ) ) );
        sg_count++;
    }

    for( auto it = sub_graphs.begin(); it != sub_graphs.end(); ++it ) {
        std::cout << "LOCAL printing subgraph (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printLocal( std::cout );
        std::cout << "GLOBAL printing subgraph (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printGlobal( std::cout );
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H
