/**
    @class          eadlib::WeightedGraph
    @brief          [ADT] Directed MultiGraph
    @dependencies   eadlib::logger::Logger, eadlib::exception:corruption
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/

#ifndef EADLIB_WEIGHTEDGRAPH_H
#define EADLIB_WEIGHTEDGRAPH_H

#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include <list>

#include "../logger/Logger.h"
#include "../exception/corruption.h"

namespace eadlib {
    template<class T> class WeightedGraph {
      public:
        //Type and inner class definitions
        typedef std::unordered_map<T, size_t> EdgeWeights_t;
        struct NodeAdjacency {
            std::list<T>  childrenList = std::list<T>();  //directed edge
            EdgeWeights_t weight       = EdgeWeights_t(); //weight of edges
            std::list<T>  parentsList  = std::list<T>();  //reverse lookup of directed edge
        };
        typedef std::unordered_map<T, NodeAdjacency> Graph_t;
        //Constructors/Destructor
        WeightedGraph();
        WeightedGraph( std::initializer_list<T> list );
        WeightedGraph( const WeightedGraph<T> &graph );
        WeightedGraph( WeightedGraph<T> &&graph );
        ~WeightedGraph() {};
        //Iterator
        typedef typename Graph_t::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;
        //Graph access
        const_iterator find( const T &node ) const;
        const NodeAdjacency & at( const T &node ) const;
        //Graph manipulation
        bool createDirectedEdge( const T &from, const T &to );
        bool createDirectedEdge( const T &from, const T &to, const size_t &weight );
        bool createDirectedEdge_fast( const T &from, const T &to );
        bool createDirectedEdge_fast( const T &from, const T &to, const size_t &weight );
        bool deleteDirectedEdge( const T &from, const T &to );
        bool deleteAllDirectedEdges( const T &from, const T &to );
        bool addNode( const T &node );
        bool deleteNode( const T &n );
        //Graph state
        bool isReachable( const T &from, const T &to ) const;
        bool nodeExists( const T &node ) const;
        bool edgeExists( const T &from, const T &to ) const;
        bool isEmpty() const;
        size_t getWeight( const T &from, const T &to );
        size_t nodeCount() const;
        size_t size() const; //Edge count
        size_t getInDegree( const T &node ) const;
        size_t getOutDegree( const T &node ) const;
        size_t getInDegree_weighted( const T &node );
        size_t getOutDegree_weighted( const T &node );
        //Print out
        std::ostream & printAdjacencyList( std::ostream &out ) const;
        std::ostream & printGraphNodes( std::ostream &out ) const;
        std::ostream & printStats( std::ostream &out ) const;
      private:
        bool checkNodesExist( const T &a, const T &b ) const;
        template <class U> bool checkOverflow( U a, U b ) const;
        Graph_t _adjacencyList;
        size_t  _edgeCount;
    };

    //-----------------------------------------------------------------------------------------------------------------
    // WeightedGraph class method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Constructor (Default)
     */
    template<class T> WeightedGraph<T>::WeightedGraph() :
        _edgeCount( 0 )
    {}

    /**
     * Constructor
     * @param list Initializer_list of Node keys
     */
    template<class T> WeightedGraph<T>::WeightedGraph( std::initializer_list<T> list ) :
        _edgeCount( 0 )
    {
        for( typename std::initializer_list<T>::iterator it = list.begin(); it != list.end(); ++it ) {
            _adjacencyList.insert( typename Graph_t::value_type( *it, NodeAdjacency() ) );
        }
    }

    /**
     * Copy-Constructor
     * @param graph Graph
     */
    template<class T> WeightedGraph<T>::WeightedGraph( const WeightedGraph<T> &graph ) :
        _edgeCount( graph._edgeCount ),
        _adjacencyList( graph._adjacencyList )
    {}

    /**
     * Move-Constructor
     * @param graph Graph
     */
    template<class T> WeightedGraph<T>::WeightedGraph( WeightedGraph<T> &&graph ) :
        _edgeCount( graph._edgeCount ),
        _adjacencyList( graph._adjacencyList )
    {}


    /**
     * const iterator begin()
     * @return begin() iterator to the Graph's adjacency list
     */
    template<class T> typename WeightedGraph<T>::const_iterator WeightedGraph<T>::begin() const{
        return _adjacencyList.cbegin();
    }

    /**
     * const iterator cend()
     * @return end() iterator to the Graph's adjacency list
     */
    template<class T> typename WeightedGraph<T>::const_iterator WeightedGraph<T>::end() const {
        return _adjacencyList.cend();
    }

    /**
     * Finds node in graph
     * @param node Node to look up in the graph
     * @return Const iterator to the Adjacency lists of the node
     */
    template<class T> typename WeightedGraph<T>::const_iterator WeightedGraph<T>::find( const T &node ) const {
        return _adjacencyList.find( node );
    }

    /**
     * Gets the Node in the graph (read-only)
     * @param node Node to look up in the graph
     * @return Adjacency lists for the node
     * throws std::out_of_range when node specified is not in the graph
     */
    template<class T> const typename WeightedGraph<T>::NodeAdjacency & WeightedGraph<T>::at( const T &node ) const {
        try {
            return _adjacencyList.at( node );
        } catch( std::out_of_range ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::at( ", node, " )] Node is not in graph." );
            throw std::out_of_range( "[eadlib::WeightedGraph<T>::at(..)] Node is not in graph." );
        }
    }

    /**
     * Links two directed nodes together
     * @param from Origin node for the directed edge
     * @param to   Destination node for the directed edge
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool WeightedGraph<T>::createDirectedEdge( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, " )]  Node(s) missing from graph." );
            return false;
        }
        if( checkOverflow<size_t>( _edgeCount,  1 ) ) {
            LOG_FATAL( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, " )] Edge count == size_t type limit. Edge not added." );
            throw std::overflow_error( "Total edge weight has reached the limit of size_t type." );
        }
        //Checking/Inserting 'to' node in children list of 'from'
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to == _adjacencyList.at( from ).childrenList.end() ) {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
        }
        //Checking/Inserting 'from' in parents list of 'to';
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        //Checking/Updating weight value for edge
        if( _adjacencyList.at( from ).weight.find( to ) == _adjacencyList.at( from ).weight.end() ) {
            _adjacencyList.at( from ).weight.emplace( typename EdgeWeights_t::value_type( to, 1 ) );
        } else {
            _adjacencyList.at( from ).weight.at( to )++;
        }
        _edgeCount++;
        return true;
    }

    /**
     * Links two directed nodes together
     * @param from   Origin node for the directed edge
     * @param to     Destination node for the directed edge
     * @param weight Edge weight
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool WeightedGraph<T>::createDirectedEdge( const T &from, const T &to, const size_t &weight ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, ", ", weight, " )] Node(s) missing from graph." );
            return false;
        }
        if( checkOverflow<size_t>( _edgeCount,  weight ) ) {
            LOG_FATAL( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, ", ", weight, " )] Adding ", weight, " to the edge count would reach the size_t limit." );
            throw std::overflow_error( "Total graph edge count would reach the limit of size_t type with the given edge weight." );
        }
        //Checking/Inserting 'to' node in children list of 'from'
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to == _adjacencyList.at( from ).childrenList.end() ) {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
        }
        //Checking/Inserting 'from' in parents list of 'to';
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        //Checking/Updating weight value for edge
        if( _adjacencyList.at( from ).weight.find( to ) == _adjacencyList.at( from ).weight.end() ) {
            _adjacencyList.at( from ).weight.emplace( typename EdgeWeights_t::value_type( to, weight ) );
        } else {
            _adjacencyList.at( from ).weight.at( to ) += weight;
        }
        _edgeCount += weight;
        return true;
    }

    /**
     * Adds and links two directed nodes together
     * @param from Origin node for the directed edge
     * @param to   Destination node for the directed edge
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool WeightedGraph<T>::createDirectedEdge_fast( const T &from, const T &to ) {
        //Error control
        if( checkOverflow<size_t>( _edgeCount,  1 ) ) {
            LOG_FATAL( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, " )] Edge count == size_t type limit. Edge not added." );
            throw std::overflow_error( "Total edge weight has reached the limit of size_t type." );
        }
        //Node creation if missing
        _adjacencyList.insert( typename Graph_t::value_type( from, NodeAdjacency() ) );
        _adjacencyList.insert( typename Graph_t::value_type( to, NodeAdjacency() ) );
        //Checking/Inserting 'to' node in children list of 'from'
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to == _adjacencyList.at( from ).childrenList.end() ) {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
        }
        //Checking/Inserting 'from' in parents list of 'to';
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        //Checking/Updating weight value for edge
        if( _adjacencyList.at( from ).weight.find( to ) == _adjacencyList.at( from ).weight.end() ) {
            _adjacencyList.at( from ).weight.emplace( typename EdgeWeights_t::value_type( to, 1 ) );
        } else {
            _adjacencyList.at( from ).weight.at( to )++;
        }
        _edgeCount++;
        return true;
    }

    /**
     * Adds and links two directed nodes together with a given weight
     * @param from   Origin node for the directed edge
     * @param to     Destination node for the directed edge
     * @param weight Edge weight
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool WeightedGraph<T>::createDirectedEdge_fast( const T &from, const T &to, const size_t &weight ) {
        //Error control
        if( checkOverflow<size_t>( _edgeCount,  weight ) ) {
            LOG_FATAL( "[eadlib::WeightedGraph<T>::createDirectedEdge( ", from, ", ", to, ", ", weight, " )] Adding ", weight, " to the edge count would reach the size_t limit." );
            throw std::overflow_error( "Total graph edge count would reach the limit of size_t type with the given edge weight." );
        }
        //Node creation if missing
        _adjacencyList.insert( typename Graph_t::value_type( from, NodeAdjacency() ) );
        _adjacencyList.insert( typename Graph_t::value_type( to, NodeAdjacency() ) );
        //Checking/Inserting 'to' node in children list of 'from'
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to == _adjacencyList.at( from ).childrenList.end() ) {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
        }
        //Checking/Inserting 'from' in parents list of 'to';
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        //Checking/Updating weight value for edge
        if( _adjacencyList.at( from ).weight.find( to ) == _adjacencyList.at( from ).weight.end() ) {
            _adjacencyList.at( from ).weight.emplace( typename EdgeWeights_t::value_type( to, weight ) );
        } else {
            _adjacencyList.at( from ).weight.at( to ) += weight;
        }
        _edgeCount += weight;
        return true;
    }

    /**
     * Deletes a directed edge between two neighbouring nodes
     * @param from Origin node
     * @param to   Destination node
     * @return Success
     */
    template<class T> bool WeightedGraph<T>::deleteDirectedEdge( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Node(s) missing from graph." );
            return false;
        }
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to ==  _adjacencyList.at( from ).childrenList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Destination node '", to, "' not found in list of children." );
            return false;
        }
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Origin node '", from, "' not found in list of parents." );
            return false;
        }
        //Edge weight check
        auto search_weight = _adjacencyList.at( from ).weight.find( to );
        if( search_weight == _adjacencyList.at( from ).weight.end() || search_weight->second <= 1 ) {
            _adjacencyList.at( from ).weight.erase( search_weight );
            _adjacencyList.at( from ).childrenList.erase( search_to );
            _adjacencyList.at( to ).parentsList.erase( search_from );
        } else {
            _adjacencyList.at( from ).weight.at( to )--;
        }
        _edgeCount--;
        return true;
    }

    /**
     * Deletes all directed edge between two neighbouring nodes
     * @param from Origin node
     * @param to   Destination node
     * @return Success
     */
    template<class T> bool WeightedGraph<T>::deleteAllDirectedEdges( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Node(s) missing from graph." );
            return false;
        }
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to ==  _adjacencyList.at( from ).childrenList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Destination node '", to, "' not found in list of children." );
            return false;
        }
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Origin node '", from, "' not found in list of parents." );
            return false;
        }
        //Edge weight check
        auto search_weight = _adjacencyList.at( from ).weight.find( to );
        if( search_weight != _adjacencyList.at( from ).weight.end() ) {
            _edgeCount -= _adjacencyList.at( from ).weight.at( to );
            _adjacencyList.at( from ).weight.erase( search_weight );
        }
        _adjacencyList.at( from ).childrenList.erase( search_to );
        _adjacencyList.at( to ).parentsList.erase( search_from );
        return true;
    }

    /**
     * Adds a node to the graph
     * @param node Node to add to graph
     * @param Success
     */
    template<class T> bool WeightedGraph<T>::addNode( const T &node ) {
        auto search = _adjacencyList.find( node );
        if( search == _adjacencyList.end() ) {
            _adjacencyList.insert( typename Graph_t::value_type( node, NodeAdjacency() ) );
            return true;
        } else {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::addNode( ", node, " )] Node is already in graph." );
            return false;
        }
    }

    /**
     * Deletes a node and all vertices (edges) connected to it
     * @param n Index of nodes to delete
     * @return Success
     */
    template<class T> bool WeightedGraph<T>::deleteNode( const T &n ) {
        auto search = _adjacencyList.find( n );
        if( search == _adjacencyList.end() ) {
            LOG_WARNING( "[eadlib::WeightedGraph<T>::deleteNode( ", n, " )] Node doesn't exist." );
            return false;
        } else {
            for ( auto node_it = _adjacencyList.begin(); node_it != _adjacencyList.end(); ++node_it ) {
                for( auto child_it = node_it->second.childrenList.begin(); child_it != node_it->second.childrenList.end(); ) {
                    if( *child_it == n ) {
                        child_it = node_it->second.childrenList.erase( child_it );
                        if( node_it->second.weight.find( n ) != node_it->second.weight.end() ) {
                            _edgeCount -= node_it->second.weight.at( n );
                            node_it->second.weight.erase( n );
                        }
                    } else {
                        ++child_it;
                    }
                }
                for( auto parent_it = node_it->second.parentsList.begin(); parent_it != node_it->second.parentsList.end(); ) {
                    if( *parent_it == n ) {
                        parent_it = node_it->second.parentsList.erase( parent_it );
                    }
                    else {
                        ++parent_it;
                    }
                }
            }
            for( auto child_weight : search->second.weight ) {
                _edgeCount -= child_weight.second;
            }
            _adjacencyList.erase( n );
            return true;
        }
    }

    /**
     * Checks if a node is reachable from another node
     * @param from_key Key of origin node
     * @param to_key   Key of node to reach
     * @return Reachable state
     * @throws eadlib::exception::corruption when a node is missing but is referenced in another node's adjacency list
     */
    template<class T> bool WeightedGraph<T>::isReachable( const T &from, const T &to ) const {
        if( from == to ) return true;
        auto search_from = _adjacencyList.find( from );
        auto search_to   = _adjacencyList.find( to );
        //Error control
        if( search_from == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::isReachable( ", from, ", ", to, ")] '", from, "' node does not exist in graph.");
            return false;
        }
        if( search_to == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::isReachable( ", from, ", ", to, ")] '", to, "' node does not exist in graph.");
            return false;
        }
        //DFS lookup
        if( search_from->second.childrenList.size() < 1 ) return false;
        std::unordered_map<T, bool> visited;
        for( typename Graph_t::const_iterator it = _adjacencyList.cbegin(); it != _adjacencyList.cend(); ++it ) {
            visited.emplace( it->first, false );
        }
        std::list<T> queue;
        queue.push_back( search_from->first );
        while( !queue.empty() ) {
            T s = queue.front();
            queue.pop_front();
            auto search_node = _adjacencyList.find( s );
            if( search_node == _adjacencyList.end() ) {
                LOG_ERROR( "[eadlib::WeightedGraph<T>::isReachable( ", from, ", ", to, " )] Node '", s, "' does not exist, yet has a directed edge pointing to it." );
                throw new eadlib::exception::corruption( "[eadlib::WeightedGraph<T>::isReachable(..)] A directed edge points to a non-existent node." );
            }
            for( auto i : search_node->second.childrenList ) {
                if( i == to && search_node->second.weight.at( to ) > 0 ) return true;
                if( !visited.at( i ) ) {
                    visited.at( i ) = true;
                    queue.push_back( i );
                }
            }
        }
        return false;
    }

    /**
     * Checks if a node exists inside the graph
     * @param node Key of node to check
     * @return Node existence
     */
    template<class T> bool WeightedGraph<T>::nodeExists( const T &node ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::nodeExists()( ", node, " )] No nodes in graph." );
            return false;
        }
        return _adjacencyList.find( node ) != _adjacencyList.cend();
    }

    /**
     * Checks if a direct directed edge exists between two nodes
     * @param from Key of origin node
     * @param to   Key of destination node
     * @return Edge existence
     */
    template<class T> bool WeightedGraph<T>::edgeExists( const T &from, const T &to ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::edgeExists()( ", from, ", ", to, " )] No nodes in graph." );
            return false;
        }
        auto search_from = _adjacencyList.find( from );
        if( search_from == _adjacencyList.cend() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::edgeExists()( ", from, ", ", to, " )] Origin node '", from, "' not found." );
            return false;
        }
        auto search_to = _adjacencyList.find( to );
        if( search_to == _adjacencyList.cend() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::edgeExists()( ", from, ", ", to, " )] Destination node '", to, "' not found." );
            return false;
        }
        auto find_result = std::find( search_from->second.childrenList.cbegin(), search_from->second.childrenList.cend(), to );
        return find_result != search_from->second.childrenList.cend();
    }

    /**
     * Checks if Graph is empty
     * @return Empty state
     */
    template<class T> bool WeightedGraph<T>::isEmpty() const {
        return _adjacencyList.empty();
    }

    /**
     * Gets the weight of an edge
     * @param from Key of origin node
     * @param to   Key of destination node
     * @return Weight of edge (0 if edge doesn't exist)
     */
    template<class T> size_t WeightedGraph<T>::getWeight( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::getWeight()( ", from, ", ", to, " )] Node(s) not in graph." );
            return 0;
        }
        auto search_from_children = std::find( _adjacencyList.at( from ).childrenList.cbegin(),
                                               _adjacencyList.at( from ).childrenList.cend(),
                                               to );
        if( search_from_children == _adjacencyList.at( from ).childrenList.cend() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::getWeight()( ", from, ", ", to, " )] Edge '", from, "'->'", to, "' not found." );
            return 0;
        }
        if( _adjacencyList.at( from ).weight.find( to ) == _adjacencyList.at( from ).weight.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::getWeight()( ", from, ", ", to, " )] Edge weight not found! Creating a value of <0>..." );
            _adjacencyList.at( from ).weight.insert( typename EdgeWeights_t::value_type( to, 0 ) );
            return 1;
        } else { //Getting the weight
            return _adjacencyList.at( from ).weight.at( to );
        }
    }

    /**
     * Gets the number of nodes in the graph
     * @return Number of nodes
     */
    template<class T> size_t WeightedGraph<T>::nodeCount() const {
        return _adjacencyList.size();
    }

    /**
     * Gets the number of edges in the graph
     * @return Number of edges (links between the nodes)
     */
    template<class T> size_t WeightedGraph<T>::size() const {
        return _edgeCount;
    }

    /**
     * Gets the number of individual node that have edge(s) coming into the node
     * @param node Node
     * @return In degree of the node
     */
    template<class T> size_t WeightedGraph<T>::getInDegree( const T &node ) const {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::getInDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        return _adjacencyList.at( node ).parentsList.size();
    }

    /**
     * Gets the number individual node that have edge(s) going out of the node
     * @param node Node
     * @return Out degree of the node
     */
    template<class T> size_t WeightedGraph<T>::getOutDegree( const T &node ) const {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::getOutDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        return _adjacencyList.at( node ).childrenList.size();
    }

    /**
     * Gets the number of edges coming into the node
     * @param node Node
     * @return In degree of the node
     */
    template<class T> size_t WeightedGraph<T>::getInDegree_weighted( const T &node ) {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::getInDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        size_t edge_counter { 0 };
        for( const T &parent : _adjacencyList.at( node ).parentsList ) {
            edge_counter += getWeight( parent, node );
        }
        return edge_counter;
    }

    /**
     * Gets the number of edges going out of the node
     * @param node Node
     * @return Out degree of the node
     */
    template<class T> size_t WeightedGraph<T>::getOutDegree_weighted( const T &node ) {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::getOutDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        if( _adjacencyList.at( node ).childrenList.size() < 1 ) {
            return 0;
        }
        size_t edge_counter { 0 };
        for( const T &child : _adjacencyList.at( node ).childrenList ) {
            edge_counter += getWeight( node, child );
        }
        return edge_counter;
    }

    /**
     * Gets the adjacency list in printable format
     * @return Output string stream of adjacency list
     */
    template<class T> std::ostream & WeightedGraph<T>::printAdjacencyList( std::ostream &out ) const {
        for( typename Graph_t::const_iterator it = _adjacencyList.cbegin(); it != _adjacencyList.cend(); ++it ) {
            out << "[" << it->first << "] -> ";
            for( T node : it->second.childrenList ) {
                out << "[" << node << "]x" << it->second.weight.at( node ) << " ";
            }
            if( it != _adjacencyList.cend()) out << "\n";
        }
        return out;
    }

    /**
     * Gets the list of all nodes in the graph in printable format
     * @return Output string stream list of Nodes
     */
    template<class T> std::ostream & WeightedGraph<T>::printGraphNodes( std::ostream &out ) const {
        for( typename Graph_t::const_iterator it = _adjacencyList.cbegin(); it != _adjacencyList.cend(); ++it ) {
            out << it->first;
            if( it != _adjacencyList.cend() ) out << "\n";
        }
        return out;
    }

    /**
     * Gets the graph stats in a printable format
     * @return Output string stream of the number of nodes and edges
     */
    template<class T> std::ostream & WeightedGraph<T>::printStats( std::ostream &out ) const {
        out << "Number of nodes: " << nodeCount() << "\n";
        out << "Number of edges: " << size() << "\n";
        return out;
    }

    /**
     * Check nodes exists
     * @param from Origin node
     * @param to   Destination node
     * @return Existence state
     */
    template<class T> bool WeightedGraph<T>::checkNodesExist( const T &from, const T &to ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::checkNodesExist( ", from, ", ", to, " )] Graph is empty." );
            return false;
        }
        if( _adjacencyList.find( from ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::checkNodesExist( ", from, ", ", to, " )] '", from, "' node not found." );
            return false;
        }
        if( _adjacencyList.find( to ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::WeightedGraph<T>::checkNodesExist( ", from, ", ", to, " )] '", to, "' node not found." );
            return false;
        }
        return true;
    }

    /**
     * Checks numerical overflow when 2 variables are to be added
     * @param a Variable a
     * @param b Variable b
     * @return Overflow state
     */
    template<class T> template<class U> bool WeightedGraph<T>::checkOverflow( U a, U b ) const {
        return ( std::numeric_limits<U>::max() - a ) < b;
    }
}

#endif //EADLIB_WEIGHTEDGRAPH_H
