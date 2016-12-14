#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHCOMPRESSOR_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHCOMPRESSOR_H

#include <unordered_map>
#include <queue>
#include <string>

#include "eadlib/cli/graphic/ProgressBar.h"
#include <eadlib/datastructure/WeightedGraph.h>

namespace sbp {
    namespace algo {
        class GraphCompressor {
          public:
            GraphCompressor( eadlib::WeightedGraph<std::string> &graph );
            ~GraphCompressor();
            void compress();
          private:
            typedef typename eadlib::WeightedGraph<std::string>::const_iterator GraphIterator_t;

            size_t compress( const GraphIterator_t &current );
            const GraphIterator_t seek( const GraphIterator_t &current,
                                        const size_t &previous_weight ) const;
            const GraphIterator_t seek( const GraphIterator_t &previous,
                                        const GraphIterator_t &current,
                                        const size_t &previous_weight ) const;
            bool validateCandidate( const GraphIterator_t &candidate,
                                    const size_t &upstream_weight );

            eadlib::WeightedGraph<std::string> & _graph;
            std::vector<std::string> _vector_of_kmers;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHCOMPRESSOR_H
