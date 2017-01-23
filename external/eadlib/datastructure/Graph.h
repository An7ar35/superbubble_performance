/**
    @class          eadlib::Graph
    @brief          [ADT] Directed %Graph

    Uses an unordered_map to create adjacency lists with reverse lookup capability
    - Map Key:     the node <T>
    - Map Value:   container that has:
                  - list of the children of the node
                  - list of the parents of the node

    @dependencies   eadlib::logger::Logger, eadlib::exception::corruption
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_GRAPH_H
#define EADLIB_GRAPH_H

#include <iostream>
#include <initializer_list>
#include <bits/stl_list.h>
#include <algorithm>
#include <vector>
#include <list>
#include <unordered_map>

#include "../logger/Logger.h"
#include "../exception/corruption.h"

namespace eadlib {
    template<class T> class Graph {
      public:
        //Type and inner class definitions
        struct NodeAdjacency {
            std::list<T> childrenList = std::list<T>(); //directed edge
            std::list<T> parentsList = std::list<T>(); //reverse lookup of directed edge
        };
        typedef std::unordered_map<T, NodeAdjacency> Graph_t;
        //Constructors/Destructor
        Graph( const std::string &name = "graph" );
        Graph( std::initializer_list<T> list );
        Graph( const Graph<T> &graph );
        Graph( Graph<T> &&graph );
        virtual ~Graph() {};
        //Iterator
        typedef typename Graph_t::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;
        //Graph access
        const_iterator find( const T &node ) const;
        const NodeAdjacency & at( const T &node ) const;
        //Graph manipulation
        bool createDirectedEdge( const T &from, const T &to );
        bool createDirectedEdge_fast( const T &from, const T &to );
        bool deleteDirectedEdge( const T &from, const T &to );
        bool addNode( const T &node );
        bool deleteNode( const T &node );
        //Graph state
        bool isReachable( const T &from, const T &to ) const;
        bool nodeExists( const T &node ) const;
        bool edgeExists( const T &from, const T &to ) const;
        bool isEmpty() const;
        size_t nodeCount() const;
        size_t size() const; //Edge count
        size_t getInDegree( const T &node ) const;
        size_t getOutDegree( const T &node ) const;
        void setName( const std::string &name );
        std::string getName() const;
        //Print out
        std::ostream & printAdjacencyList( std::ostream &out ) const;
        std::ostream & printGraphNodes( std::ostream &out ) const;
        std::ostream & printStats( std::ostream &out ) const;
      protected:
        bool checkNodesExist( const T& from, const T &to ) const;
        Graph_t     _adjacencyList;
        size_t      _edgeCount;
        std::string _name;
    };

    //-----------------------------------------------------------------------------------------------------------------
    // Graph class method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Constructor (Default)
     */
    template<class T> Graph<T>::Graph( const std::string &name ) :
        _edgeCount( 0 ),
        _name( name )
    {}

    /**
     * Constructor
     * @param list Initializer_list of Node keys
     */
    template<class T> Graph<T>::Graph( std::initializer_list<T> list ) :
        _edgeCount( 0 ),
        _name( "graph" )
    {
        for( typename std::initializer_list<T>::iterator it = list.begin(); it != list.end(); ++it ) {
            _adjacencyList.insert( typename Graph_t::value_type( *it, NodeAdjacency() ) );
        }
    }

    /**
     * Copy-Constructor
     * @param graph Graph
     */
    template<class T> Graph<T>::Graph( const Graph<T> &graph ) :
        _edgeCount( graph._edgeCount ),
        _adjacencyList( graph._adjacencyList ),
        _name( graph._name )
    {}

    /**
     * Move-Constructor
     * @param graph Graph
     */
    template<class T> Graph<T>::Graph( Graph<T> &&graph ) :
        _edgeCount( graph._edgeCount ),
        _adjacencyList( graph._adjacencyList ),
        _name( graph._name )
    {}


    /**
     * const iterator begin()
     * @return begin() iterator to the Graph's adjacency list
     */
    template<class T> typename Graph<T>::const_iterator Graph<T>::begin() const{
        return _adjacencyList.cbegin();
    }

    /**
     * const iterator cend()
     * @return end() iterator to the Graph's adjacency list
     */
    template<class T> typename Graph<T>::const_iterator Graph<T>::end() const {
        return _adjacencyList.cend();
    }

    /**
     * Finds node in graph
     * @param node Node to look up in the graph
     * @return Const iterator to the Adjacency lists of the node
     */
    template<class T> typename Graph<T>::const_iterator Graph<T>::find( const T &node ) const {
       return _adjacencyList.find( node );
    }

    /**
     * Gets the Node in the graph (read-only)
     * @param node Node to look up in the graph
     * @return Adjacency lists for the node
     * throws std::out_of_range when node specified is not in the graph
     */
    template<class T> const typename Graph<T>::NodeAdjacency & Graph<T>::at( const T &node ) const {
        try {
            return _adjacencyList.at( node );
        } catch( std::out_of_range ) {
            LOG_ERROR( "[eadlib::Graph<T>::at( ", node, " )] Node is not in graph." );
            throw std::out_of_range( "[eadlib::Graph<T>::at(..)] Node is not in graph." );
        }
    }

    /**
     * Links two directed nodes together
     * @param from Origin node for the directed edge
     * @param to   Destination node for the directed edge
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool Graph<T>::createDirectedEdge( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] Node(s) missing in graph." );
            return false;
        }
        if( _edgeCount == std::numeric_limits<size_t>::max() ) {
            LOG_FATAL( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] Edge count == size_t type limit. Nothing done." );
            throw std::overflow_error( "Total edge weight has reached the limit of size_t type." );
        }
        //Edge creation
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to != _adjacencyList.at( from ).childrenList.end() ) {
            LOG_WARNING( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] An existing child edge was found at '", from, "'." );
        } else {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
            _edgeCount++;
        }
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from != _adjacencyList.at( to ).parentsList.end() ) {
            LOG_WARNING( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] An existing parent edge was found at '", to, "'." );
        } else {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        return true;
    }

    /**
     * Add and links two directed nodes together
     * @param from Origin node for the directed edge
     * @param to   Destination node for the directed edge
     * @return Success
     * @throws std::overflow_error when the weight of the edge exceeds size_t type limit
     */
    template<class T> bool Graph<T>::createDirectedEdge_fast( const T &from, const T &to ) {
        //Error control
        if( _edgeCount == std::numeric_limits<size_t>::max() ) {
            LOG_FATAL( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] Edge count == size_t type limit. Nothing done." );
            throw std::overflow_error( "Total edge weight has reached the limit of size_t type." );
        }
        //Node creation if missing
        _adjacencyList.insert( typename Graph_t::value_type( from, NodeAdjacency() ) );
        _adjacencyList.insert( typename Graph_t::value_type( to, NodeAdjacency() ) );
        //Edge creation
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to != _adjacencyList.at( from ).childrenList.end() ) {
            LOG_WARNING( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] An existing child edge was found at '", from, "'." );
        } else {
            _adjacencyList.at( from ).childrenList.emplace_back( to );
            _edgeCount++;
        }
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from != _adjacencyList.at( to ).parentsList.end() ) {
            LOG_WARNING( "[eadlib::Graph<T>::createDirectedEdge( ", from, ", ", to, " )] An existing parent edge was found at '", to, "'." );
        } else {
            _adjacencyList.at( to ).parentsList.emplace_back( from );
        }
        return true;
    }

    /**
     * Deletes a directed edge between two neighbouring nodes
     * @param from Origin node
     * @param to   Destination node
     * @return Success
     */
    template<class T> bool Graph<T>::deleteDirectedEdge( const T &from, const T &to ) {
        //Error control
        if( !checkNodesExist( from, to ) ) {
            LOG_ERROR( "[eadlib::Graph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Node(s) missing in graph." );
            return false;
        }
        auto search_to = std::find( _adjacencyList.at( from ).childrenList.begin(),
                                    _adjacencyList.at( from ).childrenList.end(),
                                    to );
        if( search_to ==  _adjacencyList.at( from ).childrenList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Destination node '", to, "' not found in list of children." );
            return false;
        }
        auto search_from = std::find( _adjacencyList.at( to ).parentsList.begin(),
                                      _adjacencyList.at( to ).parentsList.end(),
                                      from );
        if( search_from == _adjacencyList.at( to ).parentsList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::deleteDirectedEdge( ", from, ", ", to, " )] Origin node '", from, "' not found in list of parents." );
            return false;
        }
        //Edge deletion
        _adjacencyList.at( from ).childrenList.erase( search_to );
        _adjacencyList.at( to ).parentsList.erase( search_from );
        _edgeCount--;
        return true;
    }

    /**
     * Adds a node to the graph
     * @param node Node to add to graph
     * @param Success
     */
    template<class T> bool Graph<T>::addNode( const T &node ) {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            _adjacencyList.insert( typename Graph_t::value_type( node, NodeAdjacency() ) );
            return true;
        } else {
            LOG_ERROR( "[eadlib::Graph<T>::addNode( ", node, " )] Node is already in graph." );
            return false;
        }
    }

    /**
     * Deletes a node and all vertices (edges) connected to it
     * @param nodeIndex Index of nodes to delete
     * @return Success
     */
    template<class T> bool Graph<T>::deleteNode( const T &node ) {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_WARNING( "[eadlib::Graph<T>::deleteNode( ", node, " )] Node doesn't exist." );
            return false;
        } else {
            for ( auto node_it = _adjacencyList.begin(); node_it != _adjacencyList.end(); ++node_it ) {
                for( auto child_it = node_it->second.childrenList.begin(); child_it != node_it->second.childrenList.end(); ) {
                    if( *child_it == node ) {
                        child_it = node_it->second.childrenList.erase( child_it );
                        _edgeCount--;
                    } else {
                        ++child_it;
                    }
                }
                for( auto parent_it = node_it->second.parentsList.begin(); parent_it != node_it->second.parentsList.end(); ) {
                    if( *parent_it == node ) {
                        parent_it = node_it->second.parentsList.erase( parent_it );
                    }
                    else {
                        ++parent_it;
                    }
                }
            }
            _edgeCount -= _adjacencyList.at( node ).childrenList.size();
            _adjacencyList.erase( node );
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
    template<class T> bool Graph<T>::isReachable( const T &from, const T &to ) const {
        if( from == to ) return true;
        auto search_from = _adjacencyList.find( from );
        auto search_to   = _adjacencyList.find( to );
        //Error control
        if( search_from == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::isReachable( ", from, ", ", to, ")] '", from, "' node does not exist in graph.");
            return false;
        }
        if( search_to == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::isReachable( ", from, ", ", to, ")] '", to, "' node does not exist in graph.");
            return false;
        }
        //DFS lookup
        if( search_from->second.childrenList.size() < 1 ) {
            return false;
        }
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
                LOG_ERROR( "[eadlib::Graph<T>::isReachable( ", from, ", ", to, " )] Node '", s, "' does not exist, yet has a directed edge pointing to it." );
                throw new eadlib::exception::corruption( "[eadlib::Graph<T>::isReachable(..)] A directed edge points to a non-existent node." );
            }
            for( auto i : search_node->second.childrenList ) {
                if( i == to ) return true;
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
    template<class T> bool Graph<T>::nodeExists( const T &node ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::Graph<T>::nodeExists()( ", node, " )] No nodes in graph." );
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
    template<class T> bool Graph<T>::edgeExists( const T &from, const T &to ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::Graph<T>::edgeExists()( ", from, ", ", to, " )] No nodes in graph." );
            return false;
        }
        if( _adjacencyList.find( from ) == _adjacencyList.cend() ) {
            LOG_ERROR( "[eadlib::Graph<T>::edgeExists()( ", from, ", ", to, " )] Origin node '", from, "' not found." );
            return false;
        }
        if( _adjacencyList.find( to ) == _adjacencyList.cend() ) {
            LOG_ERROR( "[eadlib::Graph<T>::edgeExists()( ", from, ", ", to, " )] Destination node '", to, "' not found." );
            return false;
        }
        auto find_result = std::find( _adjacencyList.at( from ).childrenList.cbegin(),
                                      _adjacencyList.at( from ).childrenList.cend(),
                                      to );
        return find_result != _adjacencyList.at( from ).childrenList.cend();
    }

    /**
     * Checks if Graph is empty
     * @return Empty state
     */
    template<class T> bool Graph<T>::isEmpty() const {
        return _adjacencyList.empty();
    }

    /**
     * Gets the number of nodes in the graph
     * @return Number of nodes
     */
    template<class T> size_t Graph<T>::nodeCount() const {
        return _adjacencyList.size();
    }

    /**
     * Gets the number of edges in the graph
     * @return Number of edges (links between the nodes)
     */
    template<class T> size_t Graph<T>::size() const {
        return _edgeCount;
    }

    /**
     * Gets the number of edges comming into the node
     * @param node Node
     * @return In degree of the node
     */
    template<class T> size_t Graph<T>::getInDegree( const T &node ) const {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::getInDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        return _adjacencyList.at( node ).parentsList.size();
    }

    /**
     * Gets the number of edges going out of the node
     * @param node Node
     * @return Out degree of the node
     */
    template<class T> size_t Graph<T>::getOutDegree( const T &node ) const {
        if( _adjacencyList.find( node ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::getOutDegree( ", node, " )] Node not found in graph." );
            return 0;
        }
        return _adjacencyList.at( node ).childrenList.size();
    }

    /**
     * Sets the name of the Graph
     * @param name Name of Graph
     */
    template<class T> void Graph<T>::setName( const std::string &name ) {
        _name = name;
    }

    /**
     * Gets the name of the Graph
     * @return Name of Graph
     */
    template<class T> std::string Graph<T>::getName() const {
        return _name;
    }

    /**
     * Gets the adjacency list in printable format
     * @return Output string stream of adjacency list
     */
    template<class T> std::ostream & Graph<T>::printAdjacencyList( std::ostream &out ) const {
        for( typename Graph_t::const_iterator it = _adjacencyList.cbegin(); it != _adjacencyList.cend(); ++it ) {
            out << "[" << it->first << "] -> ";
            for( T node : it->second.childrenList ) {
                out << "[" << node << "] ";
            }
            if( it != _adjacencyList.cend()) out << "\n";
        }
        return out;
    }

    /**
     * Gets the list of all nodes in the graph in printable format
     * @return Output string stream list of Nodes
     */
    template<class T> std::ostream & Graph<T>::printGraphNodes( std::ostream &out ) const {
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
    template<class T> std::ostream & Graph<T>::printStats( std::ostream &out ) const {
        out << "Number of nodes: " << nodeCount() << "\n";
        out << "Number of edges: " << size() << "\n";
        return out;
    }

    /**
     * Checks nodes exist in graph
     * @param from Origin node
     * @param to   Destination node
     * @return Existence state
     */
    template<class T> bool Graph<T>::checkNodesExist( const T &from, const T &to ) const {
        if( _adjacencyList.empty() ) {
            LOG_ERROR( "[eadlib::Graph<T>::checkNodesExist( ", from, ", ", to, " )] Graph is empty." );
            return false;
        }
        if( _adjacencyList.find( from ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::checkNodesExist( ", from, ", ", to, " )] '", from, "' node not found." );
            return false;
        }
        if( _adjacencyList.find( to ) == _adjacencyList.end() ) {
            LOG_ERROR( "[eadlib::Graph<T>::checkNodesExist( ", from, ", ", to, " )] '", to, "' node not found." );
            return false;
        }
        return true;
    }
}

#endif //EADLIB_GRAPH_H
