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
{
    findSCCs();
    LOG_DEBUG( "[sbp::algo::Tarjan(..)] Found ", _scc.size(), " strongly connected components." );
}

/**
 * Destructor
 */
sbp::algo::Tarjan::~Tarjan() {}

/**
 * Iterator begin()
 * @return Iterator
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::begin() {
    return _scc.begin();
}

/**
 * Iterator end()
 * @return Iterator
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::end() {
    return _scc.end();
}

/**
 * Const iterator begin()
 * @return Const Iterator
 */
sbp::algo::Tarjan::const_iterator sbp::algo::Tarjan::cbegin() {
    return _scc.cbegin();
}

/**
 * Const iterator end()
 * @return Iterator
 */
sbp::algo::Tarjan::const_iterator sbp::algo::Tarjan::cend() {
    return _scc.cend();
}

/**
 * Reverse iterator begin()
 * @return Reverse Iterator
 */
sbp::algo::Tarjan::reverse_iterator sbp::algo::Tarjan::rbegin() {
    return _scc.rbegin();
}

/**
 * Reverse iterator end()
 * @return Reverse Iterator
 */
sbp::algo::Tarjan::reverse_iterator sbp::algo::Tarjan::rend() {
    return _scc.rend();
}

/**
 * Const reverse iterator begin()
 * @return Const Reverse Iterator
 */
sbp::algo::Tarjan::const_reverse_iterator sbp::algo::Tarjan::rcbegin() {
    return _scc.crbegin();
}

/**
 * Const reverse iterator end()
 * @return Const Reverse Iterator
 */
sbp::algo::Tarjan::const_reverse_iterator sbp::algo::Tarjan::rcend() {
    return _scc.crend();
}

/**
 * Removes the element at pos.
 * @param pos Iterator pointing at position to delete
 * @return Iterator following the last removed element
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::erase( sbp::algo::Tarjan::iterator pos ) {
    return _scc.erase( pos );
}

/**
 * Removes the element at pos.
 * @param pos Iterator pointing at position to delete
 * @return
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::erase( sbp::algo::Tarjan::const_iterator pos ) {
    return _scc.erase( pos );
}

/**
 * Removes the elements in the range [first; last].
 * @param first Iterator pointing at first position
 * @param last  Iterator pointing at last position
 * @return Iterator following the last removed element
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::erase( sbp::algo::Tarjan::iterator first, sbp::algo::Tarjan::iterator last ) {
    return _scc.erase( first , last );
}

/**
 * Removes the elements in the range [first; last].
 * @param first Iterator pointing at first position
 * @param last  Iterator pointing at last position
 * @return Iterator following the last removed element
 */
sbp::algo::Tarjan::iterator sbp::algo::Tarjan::erase( sbp::algo::Tarjan::const_iterator first, sbp::algo::Tarjan::const_iterator last ) {
    return _scc.erase( first , last );
}

/**
 * Gets the number of SCCs found in graph
 * @return Size of list of SCCs
 */
size_t sbp::algo::Tarjan::size() const {
    return _scc.size();
}

/**
 * Checks the empty state of the SCCs found
 * @return Empty state
 */
bool sbp::algo::Tarjan::isEmpty() const {
    return _scc.empty();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// Tarjan class private method implementations
//--------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Finds Strongly Connected Components in the Graph
 */
void sbp::algo::Tarjan::findSCCs() {
    //Error control
    if( _graph.isEmpty() ) {
        LOG_ERROR( "[sbp::algo::Tarjan::getSCCs()] Graph is empty." );
        return;
    }
    //Setting up containers...
    std::unordered_map<size_t, Discovery> discovery;
    std::stack<size_t>  stack;
    std::vector<bool>   stackMember( _graph.nodeCount(), false );
    //Finding SCCs...
    size_t index { 0 };
    for( auto v : _graph ) {
        if( discovery.find( v.first ) == discovery.end() ) { //i.e. not discovered yet
            findSCCs( v.first, index, discovery, stack, stackMember );
        }
    }
}

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
            _scc.emplace_front( scc );
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
            _scc.emplace_back( scc );
        }
    }
}
