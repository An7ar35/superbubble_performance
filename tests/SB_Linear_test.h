#ifndef SUPERBUBBLE_PERFORMANCE_SB_LINEAR_TEST_H
#define SUPERBUBBLE_PERFORMANCE_SB_LINEAR_TEST_H

#include "gtest/gtest.h"
#include "../src/algorithm/superbubble/SB_Linear.h"

TEST( SB_Linear_Tests, Topological_ordering ) {
    auto g = eadlib::WeightedGraph<size_t>( "Graph" );
    g.createDirectedEdge_fast( 0, 1 );
    g.createDirectedEdge_fast( 0, 2 );
    g.createDirectedEdge_fast( 1, 3 );
    g.createDirectedEdge_fast( 2, 1 );
    g.createDirectedEdge_fast( 2, 3 );
    g.createDirectedEdge_fast( 3, 0 );
    //SCCs
    auto found_SCCs      = sbp::algo::Tarjan( g ).findSCCs();
    //Partitioning
    auto sub_graphs      = sbp::algo::PartitionGraph().partitionSCCs( g, *found_SCCs, "Test02_SubGraph" );
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    //sub_graphs->erase( sub_graphs->begin() );
    auto dag_packages = sbp::algo::GraphToDAG().convertToDAG( *sub_graphs, "Test02_DAG" );

    auto linear = sbp::algo::SB_Linear( g );
    for( auto it = dag_packages->begin(); it != dag_packages->end(); ++it ) {
        std::vector<size_t> invOrd;
        std::vector<size_t> ordD( it->_dag.nodeCount() );
        linear.fillTopologicalOrder( it->_dag, invOrd, ordD );

        std::cout << "invOrd: ";         //0 3 5 4 2 7 9 8 6 1
        for( auto e : invOrd ) {
            std::cout << e << " ";
        }
        std::cout << std::endl;

        std::cout << "ordD: ";
        for( auto e : ordD ) {
            std::cout << e << " ";
        }
        std::cout << std::endl;

        auto candidate_list = std::list<std::shared_ptr<sbp::algo::SB_Linear::Candidate>>();
        auto pvsEntrance    = std::vector<std::shared_ptr<sbp::algo::SB_Linear::Candidate>>( it->_dag.nodeCount() );
        linear.generateCandidateList( it->_dag, invOrd, candidate_list, pvsEntrance );

        std::cout << "candidate_list: ";
        for( auto e : candidate_list ) {
            std::cout << e->_node_ID << " ";
        }
        std::cout << std::endl;

        std::cout << "pvsEntrance: ";
        for( auto e : pvsEntrance ) {
            std::cout << e->_node_ID << " ";
        }
        std::cout << std::endl;
    }

}

#endif //SUPERBUBBLE_PERFORMANCE_SB_LINEAR_TEST_H
