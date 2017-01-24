#ifndef SUPERBUBBLE_PERFORMANCE_DAG_H
#define SUPERBUBBLE_PERFORMANCE_DAG_H

#include "SubGraph.h"

namespace sbp {
    namespace graph {
        class DAG : public eadlib::Graph<size_t> {
          public:
            DAG( const std::string &name );
            ~DAG();
            //Manipulation
            void addNodes( const SubGraph &sub_graph );
            //Access
            const_iterator findLocalID( const size_t &node ) const;
            std::pair<const_iterator, const_iterator> findGlobalIDs( const size_t &node ) const;
            size_t getSourceID() const;
            size_t getTerminalID() const;
            //Translation
            size_t getGlobalID( const size_t local ) const;
            std::pair<size_t, size_t> getLocalID( const size_t global ) const;
            //Print
            std::ostream & printLocal( std::ostream &out ) const;
            std::ostream & printGlobal( std::ostream &out ) const;

          private:
            std::unordered_map<size_t, size_t>                    _local2global_map; //local to global ID lookup
            std::unordered_map<size_t, std::pair<size_t, size_t>> _global2local_map; //reverse ID lookup
            size_t                                                _entrance_node;    // r
            size_t                                                _exit_node;        // r'
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_DAG_H
