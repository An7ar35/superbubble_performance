/**
   Component Name:  eadlib::algo::Tarjan
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: Tarjan algorithm to find Strongly Connected Components in a graph
                Adpated from http://www.geeksforgeeks.org/tarjan-algorithm-find-strongly-connected-components/
   Dependencies: eadlib::Logger
                 eadlib::Graph
**/

#ifndef EADLIB_TARJAN_H
#define EADLIB_TARJAN_H

#include "../logger/Logger.h"
#include "../datastructure/Graph.h"

namespace eadlib {
    namespace algo {
        template<class T> class Tarjan {
          public:
            Tarjan( const eadlib::Graph<T> &graph );
            ~Tarjan();
            //Iterators
            typedef typename std::list<std::list<T>>::iterator iterator;
            typedef typename std::list<std::list<T>>::const_iterator const_iterator;
            iterator begin();
            iterator end();
            const_iterator cbegin() const;
            const_iterator cend() const;
            //Info
            size_t size();
            //Sorting
            template<class Comparator> void sort( Comparator comparator );

          private:
            void findSCCs();
            void findSCCs( const T &next_node, int &time, std::unordered_map<T, long> &discovery, std::unordered_map<T, long> &discovery_lowest,
                           std::stack<T> &stack, std::unordered_map<T, bool> &stackMember );
            const eadlib::Graph<T> &m_graph;
            std::list<std::list<T>> m_scc;
        };

        /**
         * Constructor
         * @param graph eadlib::Graph<T> to run the algorithm with
         */
        template<class T> Tarjan<T>::Tarjan( const eadlib::Graph<T> &graph ) :
            m_graph( graph )
        {
            findSCCs();
        }

        /**
         * Destructor
         */
        template<class T> Tarjan<T>::~Tarjan() {}

        /**
         * iterator begin()
         * @return iterator
         */
        template<class T> typename Tarjan<T>::iterator Tarjan<T>::begin() {
            return m_scc.begin();
        }

        /**
         * iterator cend()
         * @return iterator
         */
        template<class T> typename Tarjan<T>::iterator Tarjan<T>::end() {
            return m_scc.end();
        }

        /**
         * const iterator begin()
         * @return iterator
         */
        template<class T> typename Tarjan<T>::const_iterator Tarjan<T>::cbegin() const {
            return m_scc.cbegin();
        }

        /**
         * const iterator cend()
         * @return iterator
         */
        template<class T> typename Tarjan<T>::const_iterator Tarjan<T>::cend() const {
            return m_scc.cend();
        }

        /**
         * Accessor wrapper for the list sort
         * @param compare Comparison function object
         */
        template<class T> template<class Comparator> void Tarjan<T>::sort( Comparator comparator ) {
            m_scc.sort( comparator );
        }

        /**
         * Gets the number of SCC lists generated
         * @return Size of the SCC list
         */
        template<class T> size_t Tarjan<T>::size() {
            return m_scc.size();
        }

        /**
         * Finds Strongly Connected Components in the Graph
         */
        template<class T> void Tarjan<T>::findSCCs() {
            //Error control
            if( m_graph.isEmpty() ) {
                LOG_ERROR( "[eadlib::algo::Tarjan<T>::getSCCs()] Graph is empty." );
                return;
            }
            //Setting up containers...
            std::unordered_map<T, long> discovery;
            std::unordered_map<T, long> discovery_lowest;
            std::stack<T>               stack;
            std::unordered_map<T, bool> stackMember;
            //Populating containers...
            for( auto v : m_graph ) {
                discovery.insert( typename std::unordered_map<T, long>::value_type( v.first, -1 ) );
                discovery_lowest.insert( typename std::unordered_map<T, long>::value_type( v.first, -1 ) );
                stackMember.insert( typename std::unordered_map<T, bool>::value_type( v.first, false ) );
            }
            //Finding SCCs...
            int time { 0 };
            for( auto v : m_graph )
                if ( discovery.at( v.first ) == -1 )
                    findSCCs( v.first, time, discovery, discovery_lowest, stack, stackMember );
        }

        /**
         * Finds SCCs recursively using DFS
         * @param next_node Next node (vertex)
         * @param time Reach time counter
         * @param discovery Container of discovery times
         * @param discovery_lowest Container of lowest recorded discovery times
         * @param stack Stack to store the connected ancestor
         * @param stackMember Container holding the record of whether a node is a member of the stack or not
         */
        template<class T> void Tarjan<T>::findSCCs( const T &next_node, int &time, std::unordered_map<T, long> &discovery,
                                                    std::unordered_map<T, long> &discovery_lowest, std::stack<T> &stack,
                                                    std::unordered_map<T, bool> &stackMember ) {
            discovery.at( next_node ) = discovery_lowest.at( next_node ) = ++time;
            stack.push( next_node );
            stackMember.at( next_node ) = true;

            for( auto i = m_graph.at( next_node ).childrenList.begin(); i != m_graph.at( next_node ).childrenList.end(); ++i ) {
                if( discovery.at( *i ) == -1 ) {
                    findSCCs( *i, time, discovery, discovery_lowest, stack, stackMember );
                    discovery_lowest.at( next_node )  = std::min( discovery_lowest.at( next_node ), discovery_lowest.at( *i ) );
                } else if( stackMember.at( *i ) ) {
                    discovery_lowest.at( next_node ) = std::min( discovery_lowest.at( next_node ), discovery.at( *i ) );
                }
            }

            if( discovery_lowest.at( next_node ) == discovery.at( next_node ) ) {
                T node;
                std::list<T> scc;
                while( stack.top() != next_node ) {
                    node = stack.top();
                    scc.emplace_back( node );
                    stackMember.at( node ) = false;
                    stack.pop();
                }
                node = stack.top();
                scc.emplace_back( node );
                stackMember.at( node ) = false;
                stack.pop();
                m_scc.emplace_back( scc );
            }
        }
    }
}

#endif //EADLIB_TARJAN_H
