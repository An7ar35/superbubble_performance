#include "DAG.h"

/**
 * Constructor
 * @param name Name of the DAG
 */
sbp::graph::DAG::DAG( const std::string &name ) :
    eadlib::Graph<size_t>( name ),
    _entrance_node( 0 ),
    _exit_node( 1 ),
    _unique_node_count( 2 )
{
    eadlib::Graph<size_t>::addNode( 0 ); //r
    eadlib::Graph<size_t>::addNode( 1 ); //r'
}

/**
 * Destructor
 */
sbp::graph::DAG::~DAG() {}

/**
 * Adds and duplicate the Nodes between r and r' from a SubGraph
 * @param sub_graph SubGraph to add the nodes from
 */
void sbp::graph::DAG::addNodes( const sbp::graph::SubGraph &sub_graph ) {
    auto local_id1 = nodeCount();
    auto local_id2 = nodeCount() + sub_graph.nodeCount() - 2; //Node copy offset
    std::cout << "Begin: id1=" << local_id1 << ", id2=" << local_id2 << std::endl;
    for( auto it = sub_graph.begin(); it != sub_graph.end(); ++it ) {
        if( it->first != sub_graph.getSourceID() && it->first != sub_graph.getTerminalID() ) {
            _local2global_map.emplace( std::make_pair( local_id1, it->first ) );
            _local2global_map.emplace( std::make_pair( local_id2, it->first ) );
            _global2local_map.emplace( std::make_pair( it->first, std::make_pair( local_id1, local_id2 ) ) );
            eadlib::Graph<size_t>::addNode( local_id1 );
            eadlib::Graph<size_t>::addNode( local_id2 );
            local_id1++;
            local_id2++;
            _unique_node_count++;
        }
    }
    std::cout << "End: id1=" << local_id1 << ", id2=" << local_id2 << std::endl;
}

/**
 * Finds a node from its local ID
 * @param node Local ID of node to find
 * @return Graph iterator
 */
eadlib::Graph<unsigned long>::const_iterator sbp::graph::DAG::findLocalID( const size_t &node ) const {
    return find( node );
}

/**
 * Finds a the 2 local nodes from their shared global ID
 * @param node Global ID of nodes to find
 * @return Graph iterator pair
 */
std::pair<eadlib::Graph<unsigned long>::const_iterator, eadlib::Graph<unsigned long>::const_iterator>
sbp::graph::DAG::findGlobalIDs( const size_t &node ) const {
    auto it = _global2local_map.find( node );
    if( it != _global2local_map.end() ) {
        auto first = find( it->second.first );
        auto second = find( it->second.second );
        return std::make_pair( first, second );
    } else {
        return std::make_pair( end(), end() );
    }
}

/**
 * Gets the local source node ID of the subgraph (r)
 * @return Source ID r
 */
size_t sbp::graph::DAG::getSourceID() const {
    return _entrance_node;
}

/**
 * Gets the local terminal node ID of the subgraph (r')
 * @return Terminal ID r'
 */
size_t sbp::graph::DAG::getTerminalID() const {
    return _exit_node;
}

/**
 * Gets the global ID from a local one
 * @param local Local ID
 * @return Global ID
 * @throws std::out_of_range when local id passed is not in global graph (r/r'/invalid node)
 */
size_t sbp::graph::DAG::getGlobalID( const size_t local ) const {
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
std::pair<size_t, size_t> sbp::graph::DAG::getLocalID( const size_t global ) const {
    try {
        return _global2local_map.at( global );
    } catch( std::out_of_range e ) {
        throw e;
    }
}

/**
 * Gets the number of non-duplicated nodes
 * @return Unique node counts
 */
size_t sbp::graph::DAG::getUniqueNodeCount() const {
    return _unique_node_count;
}

/**
 * Prints the SubGraph with local IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::DAG::printLocal( std::ostream &out ) const {
    return printAdjacencyList( out );
}

/**
 * Prints the SubGraph with global IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::DAG::printGlobal( std::ostream &out ) const {
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
