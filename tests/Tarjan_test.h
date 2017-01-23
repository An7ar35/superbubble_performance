#ifndef SUPERBUBBLE_PERFORMANCE_TARJAN_TEST_H
#define SUPERBUBBLE_PERFORMANCE_TARJAN_TEST_H

#include "gtest/gtest.h"
#include <eadlib/datastructure/WeightedGraph.h>
#include "../src/algorithm/Tarjan.h"
#include "../src/algorithm/Tarjan.cpp"

TEST( Tarjan_Tests, SCC1_concatenate ) {
    auto graph = eadlib::WeightedGraph<size_t>( "SCC1_test" );
    graph.createDirectedEdge_fast( 0, 1 );
    graph.createDirectedEdge_fast( 1, 2 );
    graph.createDirectedEdge_fast( 1, 6 );
    graph.createDirectedEdge_fast( 2, 3 );
    graph.createDirectedEdge_fast( 2, 7 );
    graph.createDirectedEdge_fast( 3, 4 );
    graph.createDirectedEdge_fast( 3, 5 );
    graph.createDirectedEdge_fast( 4, 5 );
    graph.createDirectedEdge_fast( 5, 2 );
    graph.createDirectedEdge_fast( 5, 6 );
    graph.createDirectedEdge_fast( 6, 7 );
    graph.createDirectedEdge_fast( 7, 8 );
    auto strongly_connected_components = sbp::algo::Tarjan( graph ).findSCCs();
    //Sort
    for( auto it = strongly_connected_components->begin(); it != strongly_connected_components->end(); ++it ) {
        it->sort();
    }
    strongly_connected_components->sort(
        []( std::list<size_t> a, std::list<size_t> b ) -> bool { return a.front() < b.front(); }
    );
    //Check
    ASSERT_EQ( 2, strongly_connected_components->size() );
    ASSERT_EQ( strongly_connected_components->front(), std::list<size_t>( { 0, 1, 6, 7, 8 } ) );
    strongly_connected_components->pop_front();
    ASSERT_EQ( strongly_connected_components->front(), std::list<size_t>( { 2, 3, 4, 5 } ) );
    strongly_connected_components->pop_front();
    ASSERT_TRUE( strongly_connected_components->empty() );
}

TEST( Tarjan_Tests, SCC2_concatenate ) {
    auto g = eadlib::WeightedGraph<size_t>( "SCC2_test" );
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
    auto strongly_connected_components = sbp::algo::Tarjan( g ).findSCCs();
    for( auto it = strongly_connected_components->begin(); it != strongly_connected_components->end(); ++it ) {
        it->sort();
    }
    strongly_connected_components->sort(
        []( std::list<size_t> a, std::list<size_t> b ) -> bool { return a.front() < b.front(); }
    );
    //Check
    ASSERT_EQ( 2, strongly_connected_components->size() );
    ASSERT_EQ( strongly_connected_components->front(), std::list<size_t>( { 0, 5, 6, 7 } ) ); //Singletons
    strongly_connected_components->pop_front();
    ASSERT_EQ( strongly_connected_components->front(), std::list<size_t>( { 1, 2, 3, 4 } ) ); //SCC 1
    strongly_connected_components->pop_front();
    ASSERT_TRUE( strongly_connected_components->empty() );
}

#endif //SUPERBUBBLE_PERFORMANCE_TARJAN_TEST_H
