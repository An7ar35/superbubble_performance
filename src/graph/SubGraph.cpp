#include "SubGraph.h"

/**
 * Constructor
 * @param base_graph Master graph from which the subGraph is derived
 * @param scc Strongly connected component set to build the sub graph with
 * @param subGraph_name What to name of the sub graph
 */
sbp::graph::SubGraph::SubGraph( const eadlib::WeightedGraph<size_t> &base_graph,
                                const std::list<size_t> &scc,
                                const std::string &subGraph_name  ) :
    _node_map( scc.size() + 2 ),
    _edge_count( 0 )
{
    //add nodes
    for( auto v : scc ) {
        _sub_graph.addNode( v );
    }
    //add edges

    //TODO


}

/**
 * Destructor
 */
sbp::graph::SubGraph::~SubGraph() {}

/**
 * Gets the local source node ID of the subgraph (r)
 * @return Source ID r
 */
size_t sbp::graph::SubGraph::getSourceID() const {
    return 0;
}

/**
 * Gets the local terminal node ID of the subgraph (r')
 * @return Terminal ID r'
 */
size_t sbp::graph::SubGraph::getTerminalID() const {
    return 0;
}

sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::begin() const {
    return _sub_graph.begin();
}

sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::end() const {
    return _sub_graph.end();
}

sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::find( const size_t &node ) const {
    return _sub_graph.find( node );
}

const eadlib::Graph<size_t>::NodeAdjacency &sbp::graph::SubGraph::at( const size_t &node ) const {
    return _sub_graph.at( node );
}

size_t sbp::graph::SubGraph::getGlobalID( const size_t local ) const {
    //TODO
    return 0;
}

size_t sbp::graph::SubGraph::size() const {
    return _sub_graph.size();
}

size_t sbp::graph::SubGraph::nodeCount() const {
    return _sub_graph.nodeCount();
}
