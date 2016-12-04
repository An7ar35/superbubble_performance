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


bool sbp::algo::GraphCompressor::compress() {
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
    return false;
}


const sbp::algo::GraphCompressor::GraphIterator_t sbp::algo::GraphCompressor::seek( const GraphIterator_t &current,
                                                                                    const size_t &previous_weight ) const {
    LOG_TRACE( "[sbp::algo::GraphCompressor::seek( ", current->first, ", ", previous_weight, " )] Seeking." );
    return seek( current, current, previous_weight );
}

const sbp::algo::GraphCompressor::GraphIterator_t sbp::algo::GraphCompressor::seek( const GraphIterator_t &previous,
                                                                                    const GraphIterator_t &current,
                                                                                    const size_t &previous_weight ) const {
    if( current->second.parentsList.empty() || current->second.parentsList.size() > 1 ) {
        return previous;
    }
    if( current->second.childrenList.empty() ) {
        return previous;
    }
    if( current->second.childrenList.size() > 1 ) {
        return previous;
    }
    auto downstream_weight = previous_weight;
    if( downstream_weight == 0 ) {
        downstream_weight = current->second.weight.at( current->second.childrenList.front() );
    }
    auto parent = _in_graph.find( current->second.parentsList.front() );
    if( parent->second.weight.at( current->first ) != downstream_weight ) {
        return previous;
    } else {
        //Parent is suitable for merging
        LOG_TRACE( "[sbp::algo::GraphCompressor::seek( ", previous->first, ", ", current->first, ", ", downstream_weight, " )] "
            "Seeking up..." );
        return seek( current, parent, downstream_weight );
    }
}


size_t sbp::algo::GraphCompressor::compress( const GraphIterator_t &node ) {
    size_t count { 0 };
    LOG_TRACE( "[sbp::algo::GraphCompressor::compress( ", node->first, " )] Looking to compress." );
    const GraphIterator_t start_node = seek( node, 0 ); //get to the top of the node chain that can be merged
    LOG_TRACE( "[sbp::algo::GraphCompressor::compress( ", node->first, " )] Farthest upstream start point: '", start_node->first, "'." );
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
        LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] ", merge_queue.size(), " candidates found." );
        //Are there enough nodes to combine?
        if( merge_queue.size() > 1 ) {
            LOG_TRACE( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] Starting compression." );
            GraphIterator_t end_node  = merge_queue.back();
            LOG_TRACE( "|- merging between '", start_node->first, "' and '", end_node->first, "'.");
            std::string merged_string  = start_node->first;
            while( merge_queue.size() > 1 ) {
                LOG_TRACE( "Merging '" , merged_string, "' and '", merge_queue.front()->first, "'."  );
                merged_string += merge_queue.front()->first.back(); //adding last letter
                LOG_TRACE( "Merged string = " , merged_string );
                LOG_TRACE( "Deleting '", merge_queue.front()->first, "' from graph and queue." );
                _in_graph.deleteNode( merge_queue.front()->first );
                merge_queue.pop();
                count++;
            }
            LOG_TRACE( "Merging '" , merged_string, "' and '", merge_queue.front()->first, "'."  );
            merged_string += merge_queue.front()->first.back(); //adding last letter
            LOG_TRACE( "Merged string = " , merged_string );
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
            LOG_DEBUG( "[sbp::algo::GraphCompressor::compress( ", start_node->first, " )] Compressed ", count, " nodes into 1." );
            //Deleting the old nodes
            _in_graph.deleteNode( end_node->first );
            merge_queue.pop();
            _in_graph.deleteNode( start_node->first );
            _vector_of_kmers.emplace_back( merged_string );
        }
    }
}

bool sbp::algo::GraphCompressor::validateCandidate( const GraphIterator_t &candidate, const size_t &upstream_weight ) {
    LOG_TRACE( "[sbp::algo::GraphCompressor::validateCandidate( ", candidate->first, ", ", upstream_weight, " )] Checking candidate." );
    return ( candidate->second.parentsList.size() == 1 &&
        ( ( candidate->second.childrenList.size() == 1 &&
            candidate->second.weight.at( candidate->second.childrenList.front() ) == upstream_weight )
          || ( candidate->second.childrenList.empty() )
        ) );
}






