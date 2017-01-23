#ifndef SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H
#define SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H

#include <eadlib/io/FileWriter.h>
#include "gtest/gtest.h"
#include "../src/algorithm/PartitionGraph.h"
#include "../src/algorithm/PartitionGraph.cpp"
#include "../src/graph/SubGraph.h"
#include "../src/graph/SubGraph.cpp"
#include "../src/io/DotExport.h"

TEST( SubGraph_Tests, Constructor01 ) {
    auto g = eadlib::WeightedGraph<size_t>( "Graph" );
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
    auto found_SCCs      = sbp::algo::Tarjan( g ).findSCCs();
    auto singleton_count = sbp::algo::Tarjan::concatenateSingletonSCCs( *found_SCCs );
    //Partitioning
    auto sub_graphs      = sbp::algo::PartitionGraph();
    size_t sg_count { 0 };
    auto it = found_SCCs->begin();
    if( it != found_SCCs->end() ) { //Singleton SCCs
        sub_graphs.partitionSingletonSCCs( g, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    for( it = std::next( it ); it != found_SCCs->end(); ++it) { //All the other SCCs
        sub_graphs.partitionSCC( g, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    for( auto it = sub_graphs.begin(); it != sub_graphs.end(); ++it ) {
        auto writer = eadlib::io::FileWriter( std::string( it->getName() + ".dot" ) );
        auto dot_writer = sbp::io::DotExport<size_t>( writer );
        writer.open( true );
        dot_writer.exportToDot( *it );
        std::cout << "LOCAL printing subgraph " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printLocal( std::cout );
        std::cout << "GLOBAL printing subgraph " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printGlobal( std::cout );
    }
}

TEST( SubGraph_Tests, Constructor02 ) {
    auto g = eadlib::WeightedGraph<size_t>( "Graph" );
    g.createDirectedEdge_fast( 0, 1 );
    g.createDirectedEdge_fast( 0, 5 );
    g.createDirectedEdge_fast( 1, 2 );
    g.createDirectedEdge_fast( 1, 6 );
    g.createDirectedEdge_fast( 2, 3 );
    g.createDirectedEdge_fast( 2, 4 );
    g.createDirectedEdge_fast( 3, 4 );
    g.createDirectedEdge_fast( 4, 1 );
    g.createDirectedEdge_fast( 4, 5 );
    g.createDirectedEdge_fast( 5, 6 );
    g.createDirectedEdge_fast( 6, 7 );

    //SCCs
    auto found_SCCs      = sbp::algo::Tarjan( g ).findSCCs();
    auto singleton_count = sbp::algo::Tarjan::concatenateSingletonSCCs( *found_SCCs );
    //Partitioning
    auto sub_graphs      = sbp::algo::PartitionGraph();
    size_t sg_count { 0 };
    auto it = found_SCCs->begin();
    if( it != found_SCCs->end() ) { //Singleton SCCs
        sub_graphs.partitionSingletonSCCs( g, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    for( it = std::next( it ); it != found_SCCs->end(); ++it) { //All the other SCCs
        sub_graphs.partitionSCC( g, *it, std::string( "SubGraph" + sg_count ) );
        sg_count++;
    }
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    for( auto it = sub_graphs.begin(); it != sub_graphs.end(); ++it ) {
        auto writer = eadlib::io::FileWriter( std::string( it->getName() + ".dot" ) );
        auto dot_writer = sbp::io::DotExport<size_t>( writer );
        writer.open( true );
        dot_writer.exportToDot( *it );
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SUBGRAPH_TEST_H
