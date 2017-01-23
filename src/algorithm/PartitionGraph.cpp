#include "PartitionGraph.h"

/**
 * Constructor
 */
sbp::algo::PartitionGraph::PartitionGraph() :
    _sub_graphs( std::make_unique<std::list<graph::SubGraph>>() )
{}

/**
 * Move-constructor
 * @param p PartitionGraph instance to move over
 */
sbp::algo::PartitionGraph::PartitionGraph( sbp::algo::PartitionGraph &&p ) :
    _sub_graphs( std::move( p._sub_graphs ) )
{}

/**
 * Destructor
 */
sbp::algo::PartitionGraph::~PartitionGraph() {}

/**
 * Move assignment operator
 * @param p PartitionGraph instance to move over
 * @return PartitionGraph
 */
sbp::algo::PartitionGraph & sbp::algo::PartitionGraph::operator =( sbp::algo::PartitionGraph &&p ) {
    _sub_graphs.reset( p._sub_graphs.release() );
}

/**
 * Partitions a graph into a SubGraph based on a non-singleton SCC
 * @param base_graph    Base graph (global) from which to partition off
 * @param scc           List of nodes of an SCC
 * @param subGraph_name Name of the created SubGraph
 * @return Iterator to the created SubGraph
 */
sbp::algo::PartitionGraph::iterator sbp::algo::PartitionGraph::partitionSCC( const eadlib::WeightedGraph<size_t> &base_graph,
                                                                             const std::list<size_t> &scc,
                                                                             const std::string &subGraph_name ) {
    auto sub_graph = _sub_graphs->emplace( _sub_graphs->begin(), subGraph_name );
    auto entrance_id = sub_graph->getSourceID();
    auto exit_id     = sub_graph->getTerminalID();
    //add node
    for( auto v : scc ) {
        sub_graph->addNode( v );
    }
    //add edges
    for( auto it = sub_graph->begin(); it != sub_graph->end(); ++it ) {
        if( !( it->first == entrance_id || it->first == exit_id ) ) {
            auto v = sub_graph->getGlobalID( it->first );
            if( !base_graph.at( v ).childrenList.empty() ) {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = sub_graph->findGlobalID( u );
                    if( local_u_search != sub_graph->end() ) {
                        sub_graph->createDirectedEdge( it->first, local_u_search->first );
                    } else { //edge leaving this SCC sub-graph
                        sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
                    }
                }
            }
            if( !base_graph.at( v ).parentsList.empty() ) {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( sub_graph->findGlobalID( u ) == sub_graph->end() ) { //not in sub-graph
                        sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
                    }
                }
            }
        }
    }
    return sub_graph;
}

/**
 * Partitions a graph into a SubGraph based on a list of singleton SCCs
 * @param base_graph    Base graph (global) from which to partition off
 * @param scc           List of nodes of an SCC
 * @param subGraph_name Name of the created SubGraph
 * @return Iterator to the created SubGraph
 */
sbp::algo::PartitionGraph::iterator sbp::algo::PartitionGraph::partitionSingletonSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                                                                       const std::list<size_t> &scc,
                                                                                       const std::string &subGraph_name ) {
    auto sub_graph = _sub_graphs->emplace( _sub_graphs->begin(), subGraph_name );
    auto entrance_id = sub_graph->getSourceID();
    auto exit_id     = sub_graph->getTerminalID();
    //add node
    for( auto v : scc ) {
        sub_graph->addNode( v );
    }
    //add edges
    for( auto it = sub_graph->begin(); it != sub_graph->end(); ++it ) {
        if( !( it->first == entrance_id || it->first == exit_id ) ) {
            auto v = sub_graph->getGlobalID( it->first );
            if( base_graph.at( v ).childrenList.empty() ) {
                sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
            } else {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = sub_graph->findGlobalID( u );
                    if( local_u_search != sub_graph->end() ) {
                        sub_graph->createDirectedEdge( it->first, local_u_search->first );
                    } else { //edge leaving this SCC sub-graph
                        sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
                    }
                }
            }
            if( base_graph.at( v ).parentsList.empty() ) {
                sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
            } else {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( sub_graph->findGlobalID( u ) == sub_graph->end() ) { //not in sub-graph
                        sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
                    }
                }
            }
        }
    }
    return sub_graph;
}

/**
 * Iterator begin()
 * @return Begin iterator
 */
sbp::algo::PartitionGraph::iterator sbp::algo::PartitionGraph::begin() {
    return _sub_graphs->begin();
}

/**
 * Iterator end()
 * @return End iterator
 */
sbp::algo::PartitionGraph::iterator sbp::algo::PartitionGraph::end() {
    return _sub_graphs->end();
}

/**
 * Const Iterator begin()
 * @return Begin iterator
 */
sbp::algo::PartitionGraph::const_iterator sbp::algo::PartitionGraph::cbegin() {
    return _sub_graphs->cbegin();
}

/**
 * Const Iterator end()
 * @return End iterator
 */
sbp::algo::PartitionGraph::const_iterator sbp::algo::PartitionGraph::cend() {
    return _sub_graphs->cend();
}

/**
 * Gets the empty state of the list of SubGraph
 * @return List of SubGraph empty state
 */
bool sbp::algo::PartitionGraph::empty() const {
    return _sub_graphs->empty();
}

/**
 * Gets the number of SubGraphs created
 * @return Size of list of SubGraph
 */
size_t sbp::algo::PartitionGraph::size() const {
    return _sub_graphs->size();
}


