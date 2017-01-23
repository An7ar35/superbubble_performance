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
            std::unique_ptr<std::list<std::list<size_t>>> findSCCs();
          private:
            //Type definition
            typedef std::list<std::list<size_t>> SCCList_t;
            //Structure definition
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
            void findSCCs( const size_t &vertex_id,
                           size_t &index,
                           std::unordered_map<size_t, Discovery> &discovery,
                           std::stack<size_t> &stack,
                           std::vector<bool> &stackMember );
            //Concatenating function
            void concatenateSingletonSCCs();
            //Private variables
            const eadlib::WeightedGraph<size_t> &_graph;
            std::unique_ptr<SCCList_t> _scc;
        };
    }
}


#endif //SUPERBUBBLE_PERFORMANCE_TARJAN_H
