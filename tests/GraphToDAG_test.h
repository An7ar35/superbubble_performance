#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_TEST_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_TEST_H

#include <eadlib/io/FileWriter.h>
#include "gtest/gtest.h"
#include "../src/algorithm/GraphToDAG.h"
#include "../src/algorithm/PartitionGraph.h"
#include "../src/algorithm/Tarjan.h"
#include "../src/graph/SubGraph.h"
#include "../src/graph/DAG.h"
#include "../src/io/DotExport.h"

TEST( GraphToDAG_Tests, Test01 ) {
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
    //Partitioning
    auto sub_graphs      = sbp::algo::PartitionGraph().partitionSCCs( g, *found_SCCs, "SubGraph" );
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    for( auto it = sub_graphs->begin(); it != sub_graphs->end(); ++it ) {
        std::cout << "LOCAL printing subgraph " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printLocal( std::cout );
        std::cout << "GLOBAL printing subgraph " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printGlobal( std::cout );
    }

    sub_graphs->erase( sub_graphs->begin() );
    auto dags = sbp::algo::GraphToDAG().convertToDAG( *sub_graphs, "DAG" );

    for( auto it = dags->begin(); it != dags->end(); ++it ) {
        std::cout << "LOCAL printing DAG " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printLocal( std::cout );
        std::cout << "GLOBAL printing DAG " << it->getName() << " (r:" << it->getSourceID() << ", r':" << it->getTerminalID() << "):" << std::endl;
        it->printGlobal( std::cout );
    }

}


#endif //SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_TEST_H
