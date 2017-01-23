#include "SubGraph.h"

/**
 * Constructor
 * @param subGraph_name Name of the sub-graph
 */
sbp::graph::SubGraph::SubGraph( const std::string &name ) :
    eadlib::Graph<size_t>( name ),
    _entrance_node( 0 ),
    _exit_node( 1 )
{
    eadlib::Graph<size_t>::addNode( 0 ); //r
    eadlib::Graph<size_t>::addNode( 1 ); //r'
}

/**
 * Destructor
 */
sbp::graph::SubGraph::~SubGraph() {}

/**
 * Adds a node to the SubGraph
 * @param node Node to add
 * @return Success
 */
bool sbp::graph::SubGraph::addNode( const size_t &node ) {
    auto local_id = nodeCount();
    _local2global_map.emplace( std::make_pair( local_id, node ) );
    _global2local_map.emplace( std::make_pair( node, local_id ) );
    return eadlib::Graph<size_t>::addNode( local_id );
}

/**
 * Finds a node from its local ID
 * @param node Local ID of node to find
 * @return Graph iterator
 */
eadlib::Graph<unsigned long>::const_iterator sbp::graph::SubGraph::findLocalID( const size_t &node ) const {
    return find( node );
}

/**
 * Finds a node from its global ID
 * @param node Global ID of node to find
 * @return Graph iterator
 */
eadlib::Graph<unsigned long>::const_iterator sbp::graph::SubGraph::findGlobalID( const size_t &node ) const {
    auto it = _global2local_map.find( node );
    if( it != _global2local_map.end() ) {
        return find( it->second );
    } else {
        return end();
    }
}

/**
 * Gets the local source node ID of the subgraph (r)
 * @return Source ID r
 */
size_t sbp::graph::SubGraph::getSourceID() const {
    return _entrance_node;
}

/**
 * Gets the local terminal node ID of the subgraph (r')
 * @return Terminal ID r'
 */
size_t sbp::graph::SubGraph::getTerminalID() const {
    return _exit_node;
}

/**
 * Gets the global ID from a local one
 * @param local Local ID
 * @return Global ID
 * @throws std::out_of_range when local id passed is not in global graph (r/r'/invalid node)
 */
size_t sbp::graph::SubGraph::getGlobalID( const size_t local ) const {
    try {
        return _local2global_map.at( local );
    } catch( std::out_of_range e ) {
        throw e;
    }
}

/**
 * Gets the local ID from a global one
 * @param global Global ID
 * @return Local ID
 * @throws std::out_of_range when global id passed is not in local graph (invalid node)
 */
size_t sbp::graph::SubGraph::getLocalID( const size_t global ) const {
    try {
        return _global2local_map.at( global );
    } catch( std::out_of_range e ) {
        throw e;
    }
}

/**
 * Prints the SubGraph with local IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printLocal( std::ostream &out ) const {
    return printAdjacencyList( out );
}

/**
 * Prints the SubGraph with global IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printGlobal( std::ostream &out ) const {
    for( auto it = begin(); it != end(); ++it ) {
        if( it->first == _entrance_node ) {
            out << "[r] -> ";
        } else if( it->first == _exit_node ) {
            out << "[r'] -> ";
        } else {
            out << "[" << _local2global_map.at( it->first ) << "] -> ";
        }
        for( auto child : it->second.childrenList ) {
            if( child == _entrance_node ) {
                out << "[r] ";
            } else if( child == _exit_node ) {
                out << "[r'] ";
            } else {
                out << "[" << _local2global_map.at( child ) << "] ";
            }
        }
        if( it != end() ) out << "\n";
    }
    return out;
}
