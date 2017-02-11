/**
    @class          sbp::algo::SB_Linear
    @brief          Implementation of the O(n + m) superbubble algorithm

                    Based on: "Linear-Time Superbubble Identification Algorithm for Genome Assembly"
                    by L. Brankovic, C. S. Iliopoulos, R. Kundu, M. Mohamed, S. P. Pissis, F. Vayani,
                    Theoretical Computer Science, 2015.

    @dependencies   eadlib::WeightedGraph<T>, eadlib::Graph<T>,
                    sbp::algo::container::SuperBubble, sbp::algo::Tarjan, sbp::algo::GraphToDAG

    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
#define SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H

#include <vector>
#include <memory>

#include <eadlib/datastructure/Graph.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "container/SuperBubble.h"
#include "../Tarjan.h"
#include "../PartitionGraph.h"
#include "../GraphToDAG.h"

namespace sbp {
    namespace algo {
        class SB_Linear {
          public:
            SB_Linear( const eadlib::WeightedGraph<size_t> &graph );
            ~SB_Linear();
            bool run( std::list<container::SuperBubble> &superbubble_list );
          //private:
            struct Candidate {
                Candidate( const size_t &id, const bool &entrance_flag ) :
                    _node_ID( id ),
                    _entrance_flag( entrance_flag )
                {}
                Candidate( const size_t &id, const bool &entrance_flag, std::shared_ptr<Candidate> &previous_entrance ) :
                    _node_ID( id ),
                    _entrance_flag( entrance_flag ),
                    _previous_entrance( previous_entrance )
                {}
                size_t                     _node_ID;
                bool                       _entrance_flag;
                std::shared_ptr<Candidate> _previous_entrance;
            };

            void fillTopologicalOrder( const graph::DAG &dag,
                                       std::vector<size_t> &invOrd,
                                       std::vector<size_t> &ordD );

            void topologicalSort( const graph::DAG &dag,
                                  const size_t &v,
                                  std::vector<bool> &visited,
                                  std::stack<size_t> &order_stack );

            void generateCandidateList( const sbp::graph::DAG &dag,
                                        const std::vector<size_t> &invOrd,
                                        std::list<std::shared_ptr<Candidate>> &candidate_list,
                                        std::vector<std::shared_ptr<Candidate>> &pvsEntrance );

            void generateOutChildren( const graph::DAG &dag,
                                      const std::vector<size_t> &ordD,
                                      std::vector<size_t> &out_child );

            void generateOutParents( const graph::DAG &dag,
                                     const std::vector<size_t> &ordD,
                                     std::vector<size_t> &out_parent );

            void prepareForRMQ( const std::vector<size_t> &out_child,
                                const std::vector<size_t> &out_parent,
                                std::vector<size_t> &rmq_out_child,
                                std::vector<size_t> &rmq_out_parent );

            const eadlib::WeightedGraph<size_t> _graph;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
