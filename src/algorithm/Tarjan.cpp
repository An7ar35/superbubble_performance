#include "Tarjan.h"

//--------------------------------------------------------------------------------------------------------------------------------------------
// Tarjan class public method implementations
//--------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Constructor
 * @param graph deBruijn MultiGraph
 */
sbp::algo::Tarjan::Tarjan( const eadlib::WeightedGraph<size_t> &graph ) :
    _graph( graph )
{}

/**
 * Destructor
 */
sbp::algo::Tarjan::~Tarjan() {}

/**
 * Finds Strongly Connected Components in the Graph
 * @return List of strongly connected components with the first one being a concatenated list of singleton SCCs
 */
std::unique_ptr<sbp::algo::Tarjan::SCCList_t> sbp::algo::Tarjan::findSCCs() {
    _scc = std::make_unique<SCCList_t>();
    //Error control
    if( _graph.isEmpty() ) {
        LOG_ERROR( "[sbp::algo::Tarjan::getSCCs()] Graph is empty." );
        return std::move( _scc );
    }
    //Setting up containers...
    std::unordered_map<size_t, Discovery> discovery;
    std::stack<size_t>  stack;
    std::vector<bool>   stackMember( _graph.nodeCount(), false );
    //Finding SCCs...
    size_t index { 0 };
    for( auto it = _graph.begin(); it != _graph.end(); ++it ) {
        if( discovery.find( it->first ) == discovery.end() ) { //i.e. not discovered yet
            findSCCs( it->first, index, discovery, stack, stackMember );
        }
    }
    concatenateSingletonSCCs();
    return std::move( _scc );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// Tarjan class private method implementations
//--------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Finds SCCs recursively using DFS
 * @param vertex_id   Vertex ID
 * @param index       Reach index counter
 * @param discovery   Discovery index & low link
 * @param stack       Stack to store the connected ancestor
 * @param stackMember Container holding the record of whether a node is a member of the stack or not
 */
void sbp::algo::Tarjan::findSCCs( const size_t &vertex_id,
                                  size_t &index,
                                  std::unordered_map<size_t, Discovery> &discovery,
                                  std::stack<size_t> &stack,
                                  std::vector<bool> &stackMember ) {

    // Set the depth index for v to the smallest unused index
    auto d = discovery.find( vertex_id );
    if( d == discovery.end() ) { //never discovered
        d = discovery.insert( std::make_pair( vertex_id, Discovery( index, index ) ) ).first;
    } else { //update index
        d->second._index    = index;
        d->second._low_link = index;
    }
    index++;
    stack.push( vertex_id );
    stackMember.at( vertex_id ) = true;

    // Consider successors of v
    auto v = _graph.at( vertex_id );
    for( auto w = v.childrenList.begin(); w != v.childrenList.end(); ++w ) {
        if( discovery.find( *w ) == discovery.end() ) { //index is undefined
            // Successor w has not yet been visited; recurse on it
            findSCCs( *w, index, discovery, stack, stackMember );
            d->second._low_link = std::min( d->second._low_link, discovery.at( *w )._low_link );
        } else if( stackMember.at( *w ) ) {
            // Successor w is in stack and hence in the current SCC
            d->second._low_link = std::min( d->second._low_link, discovery.at( *w )._index );
        }
    }


    // If v is a root node, pop the stack and generate an SCC
    if( d->second._low_link == d->second._index ) {
        //start a new strongly connected component
        std::list<size_t> scc;
        if( stack.top() == vertex_id ) { //singleton SCC
            scc.emplace_back( stack.top() );
            stackMember.at( stack.top() ) = false;
            stack.pop();
            //add SCC to front of list
            _scc->emplace_front( scc );
        } else { //non-singleton SCC
            while( stack.top() != vertex_id ) {
                //add w to current SCC
                scc.emplace_front( stack.top() );
                stackMember.at( stack.top() ) = false;
                stack.pop();
            }
            scc.emplace_front( stack.top() );
            stackMember.at( stack.top() ) = false;
            stack.pop();
            //add SCC to back of list
            _scc->emplace_back( scc );
        }
    }
}

/**
 * Concatenate all singletons SCCs into the first list of SCCs passed
 * @param scc_list List of SCCs
 */
void sbp::algo::Tarjan::concatenateSingletonSCCs() {
    auto first = _scc->begin();
    auto next  = _scc->begin();
    if( first != _scc->end() && _scc->size() > 1 ) {
        if( first->size() == 1 ) {
            next++;
            while( next != _scc->end() && next->size() == 1 ) {
                first->emplace_back( next->front() );
                next = _scc->erase( next );
            }
        } else { //add empty singleton list for consistency
            _scc->emplace_front( std::list<size_t>() );
        }
    }
}
