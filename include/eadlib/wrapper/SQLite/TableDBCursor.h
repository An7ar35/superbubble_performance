/**
    @class          eadlib::TableDBCursor
    @brief          Table position cursor for the eadlib::TableDB class
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_TABLEDBCURSOR_H
#define EADLIB_TABLEDBCURSOR_H

#include <utility>
#include <cstddef>
#include <ostream>

namespace eadlib {
    class TableDBCursor {
      public:
        TableDBCursor();
        TableDBCursor( const size_t &col, const size_t &row );
        friend std::ostream &operator <<( std::ostream &out, const TableDBCursor &cursor );
        void set( const size_t &row, const size_t &col );
        size_t getRow() const;
        size_t getCol() const;
        void reset();
        bool isReset() const;
      private:
        size_t _col;
        size_t _row;
        bool _reset_flag;
    };

    //-----------------------------------------------------------------------------------------------------------------
    // TableDBCursor class public method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Constructor (default)
     */
    inline TableDBCursor::TableDBCursor() :
        _col( 0 ),
        _row( 0 ),
        _reset_flag( true )
    {}

    /**
     * Constructor
     * @param row Row index
     * @param col Column index
     */
    inline TableDBCursor::TableDBCursor( const size_t &row, const size_t &col ) :
        _col( col ),
        _row( row ),
        _reset_flag( false )
    {}

    /**
     * Output stream operator
     * @param out Output stream
     * @param cursor TableDBCursor to stream
     * @return Output stream
     */
    inline std::ostream & operator <<( std::ostream &out, const TableDBCursor &cursor ) {
        out << "(" << cursor.getCol() << "," << cursor.getRow() << ")";
        return out;
    }

    /**
     * Sets the cursor values
     * @param row Row index
     * @param col Column index
     */
    inline void TableDBCursor::set( const size_t &row, const size_t &col ) {
        _col = col;
        _row = row;
        _reset_flag = false;
    }

    /**
     * Gets the row value
     * @return Row index value
     */
    inline size_t TableDBCursor::getRow() const {
        return _row;
    }

    /**
     * Gets the column value
     * @return Column index value
     */
    inline size_t TableDBCursor::getCol() const {
        return _col;
    }

    /**
     * Reset the cursor
     */
    inline void TableDBCursor::reset() {
        _col = 0;
        _row = 0;
        _reset_flag = true;
    }

    /**
     * Check if cursor is reset
     * @return
     */
    inline bool TableDBCursor::isReset() const {
        return _reset_flag;
    }
}

#endif //EADLIB_TABLEDBCURSOR_H
