#ifndef SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H
#define SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H

#include "eadlib/io/FileWriter.h"
#include "eadlib/logger/Logger.h"
#include "eadlib/datastructure/WeightedGraph.h"
#include "../graph/SubGraph.h"

namespace sbp {
    namespace io {
        template<class T> class DotExport {
          public:
            enum class EdgeType { MULTI_EDGE, WEIGHT_LABEL};
            DotExport( eadlib::io::FileWriter &writer );
            ~DotExport();
            bool exportToDot( const std::string &graph_name,
                              const eadlib::WeightedGraph<T> &graph,
                              const bool &weight_label );
            bool exportToDot( const sbp::graph::SubGraph &sub_graph );
          private:
            eadlib::io::FileWriter &_writer;
        };

        //-----------------------------------------------------------------------------------------------------------------
        // DotExport class public method implementations
        //-----------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param writer EADLib File writer
         */
        template<class T> DotExport<T>::DotExport( eadlib::io::FileWriter &writer ) :
            _writer( writer )
        {}

        /**
         * Destructor
         */
        template<class T> DotExport<T>::~DotExport() {}


        /**
         * Exports the graph as a Dot file
         * @param graph        Graph to export
         * @param weight_label Shows multi-edges as weight labels (default=true)
         * @return Success
         */
        template<class T> bool DotExport<T>::exportToDot( const std::string &graph_name,
                                                          const eadlib::WeightedGraph<T> &graph,
                                                          const bool &weight_label ) {
            if( !_writer.isOpen() && !_writer.open() ) {
                LOG_ERROR( "[sbp::io::DotExport::exportToDot(..)] Could not open file '", _writer.getFileName(), "'." );
                return false;
            }
            _writer.write( "digraph " + graph_name + " {\n" );
            _writer.write( "\tnode [shape = circle]\n" );
            for( auto node : graph ) {
                if( node.second.childrenList.empty() && node.second.parentsList.empty() ) {
                    std::ostringstream oss;
                    oss << "\t" << node.first << ";\n";
                    _writer.write( oss.str() );
                } else {
                    for( auto dest : node.second.childrenList ) {
                        if( weight_label ) { //shows single edges with weight
                            std::ostringstream oss;
                            oss << "\t"
                                << node.first
                                << " -> "
                                << dest
                                << " [label=\""
                                <<  node.second.weight.at( dest )
                                << "\"]\n";
                            _writer.write( oss.str() );
                        } else { //shows every duplicate edges
                            for( size_t w  = 0; w < node.second.weight.at( dest ); w++ ) {
                                std::ostringstream oss;
                                oss << "\t" << node.first << " -> " << dest << "\n";
                                _writer.write( oss.str() );
                            }
                        }
                    }
                }
            }
            _writer.write( "}" );
            return true;
        }

        /**
         * Exports the subgraph to a Dot file
         * @param graph_name Name of the sub-graph
         * @param sub_graph  SubGraph instance
         * @return Success
         */
        template<class T> bool DotExport<T>::exportToDot( const sbp::graph::SubGraph &sub_graph ) {
            auto getRealID = [&]( const size_t &local_id ) {
                if( local_id == sub_graph.getSourceID() ) {
                    return std::string( "r" );
                } else if( local_id == sub_graph.getTerminalID() ) {
                    return std::string( "r\'" );
                } else {
                    return std::to_string( sub_graph.getGlobalID( local_id ) );
                }
            };

            if( !_writer.isOpen() && !_writer.open() ) {
                LOG_ERROR( "[sbp::io::DotExport::exportToDot(..)] Could not open file '", _writer.getFileName(), "'." );
                return false;
            }

            _writer.write( "digraph " + sub_graph.getName() + " {\n" );
            _writer.write( "\tnode [shape = circle]\n" );
            for( auto it = sub_graph.begin(); it != sub_graph.end(); ++it ) {
                if( it->second.childrenList.empty() && it->second.parentsList.empty() ) {
                    std::ostringstream oss;
                    oss << "\t\"" << getRealID( it->first ).c_str() << "\";\n";
                    _writer.write( oss.str() );
                } else {
                    for( auto dest : it->second.childrenList ) {
                        std::ostringstream oss;
                        oss << "\t\""
                            << getRealID( it->first ).c_str()
                            << "\" -> \""
                            << getRealID( dest ).c_str()
                            << "\"\n";
                        _writer.write( oss.str() );
                    }
                }
            }
            _writer.write( "}" );
            return true;
        }
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H
