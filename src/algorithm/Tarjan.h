/**
    @class          sbp::algo::Tarjan
    @brief          Algorithm to find SCCs in a graph

                    SCCs found are ordered so that singletons are placed
                    at the front of the list and the rest at the back

    @dependencies   eadlib::WeightedGraph<T>, eadlib::logger::Logger
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef SUPERBUBBLE_PERFORMANCE_TARJAN_H
#define SUPERBUBBLE_PERFORMANCE_TARJAN_H

#include <eadlib/logger/Logger.h>
#include <eadlib/datastructure/WeightedGraph.h>

namespace sbp {
    namespace algo {
        class Tarjan {
          public:
            Tarjan( const eadlib::WeightedGraph<size_t> &graph );
            ~Tarjan();
            //Iterators
            typedef std::list<std::list<size_t>>::iterator iterator;
            typedef std::list<std::list<size_t>>::reverse_iterator reverse_iterator;
            typedef std::list<std::list<size_t>>::const_iterator const_iterator;
            typedef std::list<std::list<size_t>>::const_reverse_iterator const_reverse_iterator;
            iterator begin();
            iterator end();
            const_iterator cbegin();
            const_iterator cend();
            reverse_iterator rbegin();
            reverse_iterator rend();
            const_reverse_iterator rcbegin();
            const_reverse_iterator rcend();
            //Properties
            size_t size();
            bool isEmpty();
            //Sort
            template<class Comparator> void sort( Comparator comparator );
          private:
            struct Discovery {
                Discovery( const size_t &index, const size_t &lowest ) :
                    _index( index ),
                    _low_link( lowest )
                {}
                ~Discovery() {};
                size_t _index;
                size_t _low_link;
            };
            //Private functions
            void findSCCs();
            void findSCCs( const size_t &vertex_id,
                           size_t &index,
                           std::unordered_map<size_t, Discovery> &discovery,
                           std::stack<size_t> &stack,
                           std::vector<bool> &stackMember );
            //Private variables
            const eadlib::WeightedGraph<size_t> &_graph;
            std::list<std::list<size_t>> _scc;
        };

        /**
         * Sorts the list of SCCs found
         * @param comparator Comparator to use for sorting
         */
        template<class Comparator> void Tarjan::sort( Comparator comparator ) {
            _scc.sort( comparator );
        }
    }
}


#endif //SUPERBUBBLE_PERFORMANCE_TARJAN_H
