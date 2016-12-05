#include <queue>
#include "GraphCompressor.h"

/**
 * Constructor
 * @param graph de Bruijn graph to collapse
 */
sbp::algo::GraphCompressor::GraphCompressor( eadlib::WeightedGraph<std::string> &in_graph ) :
    _in_graph( in_graph )
{}

/**
 * Destructor
 */
sbp::algo::GraphCompressor::~GraphCompressor() {}

/**
 * Compress the graph
 */
void sbp::algo::GraphCompressor::compress() {
    for( auto node : _in_graph ) {
        _vector_of_kmers.emplace_back( node.first );
    }
    size_t count { 0 };
    while( count < _vector_of_kmers.size() ) {
        auto it = _in_graph.find( _vector_of_kmers.at( count ) );
        if( it != _in_graph.end() ) {
            compress( it );
        }
        count++;
    }
}

/**
 * Assesses compression possibility from a node and compresses the whole chain
 * @param node Node to start from
 * @return Number of nodes compressed
 */
size_t sbp::algo::GraphCompressor::compress( const GraphIterator_t &node ) {
    size_t count { 0 };
    const GraphIterator_t start_node = seek( node, 0 ); //get to the top of the node chain that can be merged
    LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", node->first, " )] Farthest upstream start point: '", start_node->first, "'." );
    if( start_node->second.childrenList.size() == 1 ) {
        std::queue<GraphIterator_t> merge_queue;
        bool valid_candidate { true };
        GraphIterator_t current = start_node;
        //Add all attached candidates for merging to queue
        do {
            GraphIterator_t next = _in_graph.find( current->second.childrenList.front() );
            auto w = current->second.weight.at( next->first );
            valid_candidate = validateCandidate( next, w );
            if( valid_candidate ) {
                merge_queue.push( next );
                current = next;
            }
        } while( !current->second.childrenList.empty() && valid_candidate );
        //Are there enough nodes to combine?
        if( merge_queue.size() > 1 ) {
            LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] ", merge_queue.size() + 1, " candidates found in chain." );
            GraphIterator_t end_node  = merge_queue.back();
            //Combining the values of the queued nodes
            std::string merged_string  = start_node->first;
            while( merge_queue.size() > 1 ) {
                merged_string += merge_queue.front()->first.back(); //adding last letter
                _in_graph.deleteNode( merge_queue.front()->first );
                merge_queue.pop();
                count++;
            }
            merged_string += merge_queue.front()->first.back(); //adding last letter
            count++;
            //Creating new node for merged content
            _in_graph.addNode( merged_string );
            for( auto parent : start_node->second.parentsList ) {
                auto in_weight = _in_graph.at( parent ).weight.at( start_node->first );
                _in_graph.createDirectedEdge( parent, merged_string, in_weight );
            }
            for( auto child : end_node->second.childrenList ) {
                auto out_weight = end_node->second.weight.at( child );
                _in_graph.createDirectedEdge( merged_string, child, out_weight );
            }
            LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] "
                "Compressed ", ( count > 0 ? count + 1 : count ), " nodes into 1." );
            //Deleting the first and last nodes of the original chain
            _in_graph.deleteNode( end_node->first );
            merge_queue.pop();
            _in_graph.deleteNode( start_node->first );
            //_vector_of_kmers.emplace_back( merged_string ); //overkill
        } else {
            LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] Node not in a compressible chain." );
        }
    }
    return count;
}

/**
 * Seeks the top most node in a compressible chain of nodes
 * @param current Starting point node
 * @param previous_weight Weight of the current->child edge if known
 * @return GraphIterator_t to the top node
 */
const sbp::algo::GraphCompressor::GraphIterator_t sbp::algo::GraphCompressor::seek( const GraphIterator_t &current,
                                                                                    const size_t &previous_weight ) const {
    return seek( current, current, previous_weight );
}

/**
 * Seeks the top most node in a compressible chain of nodes (helper function)
 * @param previous Previous node sought
 * @param current Current node
 * @param previous_weight Weight of the current->previous edge if known
 * @return GraphIterator_t to the top node of the chain
 */
const sbp::algo::GraphCompressor::GraphIterator_t sbp::algo::GraphCompressor::seek( const GraphIterator_t &previous,
                                                                                    const GraphIterator_t &current,
                                                                                    const size_t &previous_weight ) const {
    if( current->second.parentsList.empty() || current->second.parentsList.size() > 1 ) {
        return previous;
    }
    if( current->second.childrenList.size() > 1 ) {
        return previous;
    }
    auto parent = _in_graph.find( current->second.parentsList.front() );
    if( !current->second.childrenList.empty() ) {
        auto downstream_weight = previous_weight;
        if( downstream_weight == 0 ) {
            downstream_weight = current->second.weight.at( current->second.childrenList.front() );
        }
        if( parent->second.weight.at( current->first ) != downstream_weight ) {
            return previous;
        } else {
            //Parent is suitable for merging
            return seek( current, parent, downstream_weight );
        }
    } else {
        return seek( current, parent, 0 );
    }
}

/**
 * Checks if a candidate node is good for compression
 * @param candidate Node
 * @param upstream_weight Weight from the previous edge
 * @return Validation state
 */
bool sbp::algo::GraphCompressor::validateCandidate( const GraphIterator_t &candidate, const size_t &upstream_weight ) {
    return ( candidate->second.parentsList.size() == 1 &&
        ( ( candidate->second.childrenList.size() == 1 &&
            candidate->second.weight.at( candidate->second.childrenList.front() ) == upstream_weight )
          || ( candidate->second.childrenList.empty() )
        ) );
}






