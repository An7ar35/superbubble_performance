#include "DotExport.h"

/**
 * Constructor
 * @param writer EADLib File writer
 */
sbp::io::DotExport::DotExport( eadlib::io::FileWriter &writer ) :
    _writer( writer )
{}

/**
 * Destructor
 */
sbp::io::DotExport::~DotExport() {}

/**
 * Exports the graph as a Dot file
 * @param graph        Graph to export
 * @param weight_label Shows multi-edges as weight labels (default=true)
 * @return Success
 */
bool sbp::io::DotExport::exportToDot( const eadlib::WeightedGraph<std::string> &graph, const bool &weight_label ) {
    if( !_writer.isOpen() && !_writer.open() ) {
        LOG_ERROR( "[sbp::io::DotExport::export(..)] Could not open file '", _writer.getFileName(), "'." );
        return false;
    }
    _writer.write( "digraph genome {\n" );
    _writer.write( "\tnode [shape = circle]\n" );
    for( auto node : graph ) {
        if( node.second.childrenList.empty() ) {
            _writer.write( "\t" + node.first );
        } else {
            for( auto dest : node.second.childrenList ) {
                if( weight_label ) { //shows single edges with weight
                    _writer.write( "\t"
                                   + node.first
                                   + " -> "
                                   + dest
                                   + " [label=\""
                                   + std::to_string( node.second.weight.at( dest ) )
                                   + "\"]\n" );
                } else { //shows every duplicate edges
                    for( size_t w  = 0; w < node.second.weight.at( dest ); w++ ) {
                        _writer.write( "\t" + node.first + " -> " + dest + "\n" );
                    }
                }
            }
        }
    }
    _writer.write( "}" );
    return true;
}
