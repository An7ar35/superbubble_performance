/**
    @class          eadlib::TableDB
    @brief          Multi-typed table for database query result storage

                    USER GUIDE: Add columns > Lock table structure > Add data sequentially

    @dependencies   eadlib::TableDBCell, eadlib::TableDBRow, eadlib::TableDBCol, eadlib::TableDBCursor, eadlib::logger::Logger
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_DBTABLE_H
#define EADLIB_DBTABLE_H

#include <string>
#include <vector>
#include <memory>

#include "TableDBCell.h"
#include "TableDBCol.h"
#include "TableDBRow.h"
#include "TableDBCursor.h"

namespace eadlib {
    class TableDB {
      public:
        TableDB();
        ~TableDB();
        //Iterators
        typedef std::vector<TableDBRow>::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;
        //Table access
        const TableDBCell & at( const size_t &column, const size_t &row ) const;
        //Table find
        TableDBCursor find( const TableDBCell &cell_value );
        TableDBCursor findInCol( const std::string &heading, const TableDBCell &value ) const;
        std::pair<size_t, bool> findRow( const size_t &column, const TableDBCell &value ) const;
        std::pair<size_t, bool> findColumn( const std::string &heading ) const;
        //Table sort
        template<typename Comparator> void sort( const size_t &column, Comparator comparator );
        //Stream operator
        friend std::ostream &operator <<( std::ostream &out, TableDB &table );
        //Table structure
        void reset();
        void clear();
        bool createColumn( const std::string &heading, const TableDBCell::DataType &data_type );
        void lockStructure();
        //Sequential adding of data
        bool add();
        bool add( bool value );
        bool add( signed long long value );
        bool add( double value );
        bool add( char const *value );
        bool add( std::string value );
        //Table properties
        std::string getHeading( const size_t &column ) const;

        TableDBCell::DataType getColType( const size_t &column ) const;
        size_t getColCount() const;
        size_t getRowCount() const;
        //Checks
        bool checkIntegrity(); //full data types and border limits checks
        bool checkBorders(); //border limits check

        std::pair<size_t, bool> searchFor( const std::string &column1, const TableDBCell &item1,
                                           const std::string &column2, const TableDBCell &item2 ); //order doesn't matter
      private:
        bool   _lock_flag;
        bool   _integrity_flag;
        size_t _table_size_x;
        size_t _table_size_y;
        //(x,y) coordinate of the last occupied cell
        TableDBCursor _cursor;
        //Containers
        std::vector<TableDBCol> _columns; //Headings & Types
        std::vector<TableDBRow> _rows;    //Individual rows of data
        //Private methods
        void createRow();
        void resetIntegrityFlag();
        TableDBCursor getNextCellFor( const TableDBCursor &current );
        bool checkNext( TableDBCursor &next, TableDBCell::DataType type );
        bool checkLastCellPosition();
        //Search templates

        std::pair<size_t, bool> findPair( const size_t &col1, const TableDBCell &value1, const size_t &col2, const TableDBCell &value2 );
    };

    //-----------------------------------------------------------------------------------------------------------------
    // TableDB class public method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Constructor
     */
    inline TableDB::TableDB() :
        _table_size_x( 0 ),
        _table_size_y( 0 ),
        _lock_flag( false ),
        _integrity_flag( false ),
        _cursor( TableDBCursor() )
    {}

    /**
     * Destructor
     */
    inline TableDB::~TableDB() {}

    /**
     * Const Iterator for the table (left to right, row by row downwards)
     * @return Begin() iterator
     */
    inline TableDB::const_iterator TableDB::begin() const {
        return _rows.begin();
    }

    /*
     * Const Iterator for the table (left to right, row by row downwards)
     * @return End() iterator
     */
    inline TableDB::const_iterator TableDB::end() const {
        return _rows.end();
    }

    /**
     * TableDBCell access
     * @param column Column index
     * @param row    Row index
     * @return Cell
     * @throws std::out_of_range when one/both index/indices are out of bounds
     */
    inline const TableDBCell & TableDB::at( const size_t &column, const size_t &row ) const {
        if( _rows.size() == 0 ) {
            LOG_WARNING( "[eadlib::TableDB::at( ", column, ", ", row, " )] No data in table." );
            throw std::out_of_range( "[eadlib::TableDB::at()] Index out of range" );
        } else if( row < _rows.size() && column < _rows.at( row ).getLength() ) {
            if( !_lock_flag ) {
                LOG_ERROR( "[eadlib::TableDB::at( ", column, ", ", row, " )] Trying to access Item on an unlocked Table. Please lock the Table first." );
            }
            return _rows.at( row ).at( column );
        } else {
            LOG_ERROR( "[eadlib::TableDB::at( ", column, ", ", row, " )] Index out of bounds (", _rows.size(), " x ", _rows.at( row ).getLength(), ")." );
            throw std::out_of_range( "eadlib::Table::operator() : index out of range" );
        }
    }

    /**
     * Searches the Table for a value inside a column with the specified heading
     * @param heading Heading of the column to look in
     * @param value   Value to find
     * @return Cursor to cell position of the found value (if nothing is found the cursor's reset flag will be true)
     */
    inline TableDBCursor TableDB::findInCol( const std::string &heading, const TableDBCell &value ) const {
        auto col = findColumn( heading );
        if( !col.second ) {
            LOG_ERROR( "[eadlib::TableDB::findInCol( ", heading, ", ", value, " )] Column name does not match any columns in table." );
            return TableDBCursor();
        }
        auto row = findRow( col.first, value );
        if( !row.second ) {
            return TableDBCursor();
        }
        return TableDBCursor( col.first, row.first );
    }

    /**
     * Finds a cell value in a column
     * @param column Column number
     * @param value  Cell value to find
     * @return Pair consisting of row index reached and success flag
     */
    inline std::pair<size_t, bool> TableDB::findRow( const size_t &column, const TableDBCell &value ) const {
        size_t count { 0 };
        while( count < _rows.size() ) {
            if( _rows.at( count ).at( column ) == value ) {
                return std::make_pair( count, true );
            }
            count++;
        }
        return std::make_pair( count, false );
    }

    /**
     * Finds a column heading
     * @param heading Column heading to find
     * @return Pair consisting of column index reached and success flag
     */
    inline std::pair<size_t, bool> TableDB::findColumn( const std::string &heading ) const {
        size_t count { 0 };
        for( auto it = _columns.begin(); it != _columns.end(); it++ ) {
            if( it->_heading == heading ) return std::make_pair( count, true );
            count++;
        }
        LOG_ERROR( "[eadlib::TableDB::getColIndex( ", heading, " )] Heading not found in table." );
        return std::make_pair( count, false );
    }

    /**
     * Searches the entire Table for a cell value
     * @param cell_value Value to find
     * @return Cursor to the first match found. If not found the cursor's reset flag will be true.
     */
    inline TableDBCursor TableDB::find( const TableDBCell &cell_value ) {
        for( size_t col = 0; col < _columns.size(); col++ ) {
            if( cell_value.getType() == _columns[ col ]._type ) {
                auto row = findRow( col, cell_value );
                if( row.second ) {
                    return TableDBCursor( col, row.first );
                }
            }
        }
        return TableDBCursor();
    }

    /**
     * Sort table based on a column
     * Note: lexical sort of strings does not arrange single lettered entry followed by a space properly.
     * @param column     Column index
     * @param comparator Comparator function to use for sorting
     * @throws std::out_of_range when column index is out of bound of the table
     */
    template<typename Comparator> void TableDB::sort( const size_t &column, Comparator comparator ) {
        if( column < getColCount() ) {
            std::sort( _rows.begin(), _rows.end(), [&]( TableDBRow &a, TableDBRow& b ) {
                return comparator( a.at( column ), b.at( column ) );
            } );
        } else {
            LOG_ERROR( "[eadlib::TableDB::sort( ", column, ", <Comparator> )] Column index out of range." );
            throw std::out_of_range( "[eadlib::TableDB::sort(..)] Column index out of range." );
        }
    }

    /**
     * Output stream
     * @param out  Output stream
     * @param table Table to stream out
     * @return Output stream
     */
    inline std::ostream & operator <<( std::ostream &out, TableDB &table ) {
        if( !table.checkIntegrity() ) {
            out << "Consistency check has fail. Possible reasons:" << std::endl
                << "\t- Column lengths are not all the same," << std::endl << "\t- Table is empty."
                << std::endl;
        } else {
            //Headings
            out << "| ";
            for( int i = 0; i < table.getColCount(); i++ ) {
                out << table.getHeading( i ) << " | ";
            }
            out << std::endl;
            //Main data
            if( table.getRowCount() == 0 ) {
                out << "No data in table..." << std::endl;
            } else {
                for( int i = 0; i < table.getRowCount(); i++ ) { //Rows
                    out << "| ";
                    for( int j = 0; j < table.getColCount(); j++ ) { //Columns
                        out << table.at( i, j ) << " | ";
                    }
                    out << std::endl;
                }
                out << std::endl;
            }
        }
        return out;
    }

    /**
     * Clears everything inside the table (inc. columns)
     */
    inline void TableDB::reset() {
        _lock_flag = false;
        std::vector<TableDBCol>().swap( _columns );
        std::vector<TableDBRow>().swap( _rows );
        _table_size_x = 0;
        _table_size_y = 0;
        _cursor.reset();
        resetIntegrityFlag();
    }

    /**
     * Clear the data inside the rows, leave columns alone.
     */
    inline void TableDB::clear() {
        _lock_flag = false;
        std::vector<TableDBRow>().swap( _rows );
        _table_size_y = 0;
        _cursor.reset();
        resetIntegrityFlag();
    }

    /**
     * Creates a column in the TableDB
     * @tparam Type   TableDB::DataType of the column's cells
     * @param heading Column heading
     * @return Success
     */
    inline bool TableDB::createColumn( const std::string &heading, const TableDBCell::DataType &data_type ) {
        if( _lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::createColumn<", static_cast<int>( data_type ), ">(  ", heading, " )] Table is locked." );
            return false;
        }
        if( data_type == TableDBCell::DataType::NONE ) {
            LOG_DEBUG( "[eadlib::TableDB::createColumn<DataType::NONE>( ", heading, " )] Column created with 'NULL' data type" );
        }
        resetIntegrityFlag();
        _columns.push_back( TableDBCol { data_type, heading } );
        _table_size_x++;
        return true;
    }

    /**
     * Locks the table:
     * > No more columns may be added
     * > Data can now be added into the table
     */
    inline void TableDB::lockStructure() {
        _lock_flag = true;
    }

    /**
     * Adds a Null Item to the table sequentially
     * @return Success
     */
    inline bool TableDB::add() {
        if( !_lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::add()] Table structure isn't locked. Lock table before adding data." );
            return false;
        }
        TableDBCursor next = getNextCellFor( _cursor );
        if( checkNext( next, TableDBCell::DataType::NONE ) ) {
            if( next.getRow() > _cursor.getRow() ) { //create new row then push
                createRow();
                _rows.at( next.getRow() ).add();
            } else { //push on current row
                resetIntegrityFlag();
                _rows.at( _cursor.getRow() ).add();
            }
            _cursor = next;
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::add()] Couldn't add null value." );
        return false;
    }

    /**
     * Adds an 'bool' Item to the table sequentially
     * @param value
     * @return Success
     */
    inline bool TableDB::add( bool value ) {
        if( !_lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Table structure isn't locked. Lock table before adding data." );
            return false;
        }
        TableDBCursor next = getNextCellFor( _cursor );
        if( checkNext( next, TableDBCell::DataType::BOOL ) ) {
            if( next.getRow() > _cursor.getRow() ) { //create new row then push
                createRow();
                _rows.at( next.getRow() ).add( value );
            } else { //push on current row
                resetIntegrityFlag();
                _rows.at( _cursor.getRow() ).add( value );
            }
            _cursor = next;
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Couldn't add boolean value." );
        return false;
    }

    /**
     * Adds an Integer item to the table sequentially
     * @param value
     * @return
     */
    inline bool TableDB::add( signed long long value ) {
        if( !_lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Table structure isn't locked. Lock table before adding data." );
            return false;
        }
        TableDBCursor next = getNextCellFor( _cursor );
        if( checkNext( next, TableDBCell::DataType::INT ) ) {
            if( next.getRow() > _cursor.getRow() ) { //create new row then push
                createRow();
                _rows.at( next.getRow() ).add( value );
            } else { //push on current row
                resetIntegrityFlag();
                _rows.at( _cursor.getRow() ).add( value );
            }
            _cursor = next;
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Couldn't add integer value." );
        return false;
    }

    /**
     * Adds a 'double' Item to the table sequentially
     * @param value
     * @return
     */
    inline bool TableDB::add( double value ) {
        if( !_lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Table structure isn't locked. Lock table before adding data." );
            return false;
        }
        TableDBCursor next = getNextCellFor( _cursor );
        if( checkNext( next, TableDBCell::DataType::DOUBLE ) ) {
            if( next.getRow() > _cursor.getRow() ) { //create new row then push
                createRow();
                _rows.at( next.getRow() ).add( value );
            } else { //push on current row
                resetIntegrityFlag();
                _rows.at( _cursor.getRow() ).add( value );
            }
            _cursor = next;
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Couldn't add double value." );
        return false;
    }

    /**
     * Converts a 'char' array into a std::string' Item and adds it to the table sequentially
     * @param value
     * @return
     */
    inline bool TableDB::add( char const *value ) {
        return add( std::string( value ) );
    }

    /**
     * Adds a 'std::string' Item to the table sequentially
     * @param value
     * @return
     */
    inline bool TableDB::add( std::string value ) {
        if( !_lock_flag ) {
            LOG_ERROR( "[eadlib::TableDB::add( ", value, " )] Table structure isn't locked. Lock table before adding data." );
            return false;
        }
        TableDBCursor next = getNextCellFor( _cursor );
        if( checkNext( next, TableDBCell::DataType::STRING ) ) {
            if( next.getRow() > _cursor.getRow() ) { //create new row then push
                createRow();
                _rows.at( next.getRow() ).add( value );
            } else { //push on current row
                resetIntegrityFlag();
                _rows.at( _cursor.getRow() ).add( value );
            }
            _cursor = next;
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::add( '", value, "' )] Couldn't add string value." );
        return false;
    }

    /**
     * Gets the heading of a column
     * @param column Column index
     * @return Column heading
     * @throws std::out_of_range when column index is out of bounds
     */
    inline std::string TableDB::getHeading( const size_t &column ) const {
        if( column >= getColCount() ) {
            LOG_ERROR( "[eadlib::TableDB::getHeading( ", column, " )] Column does not exist." );
            throw std::out_of_range( "[eadlib::TableDB::getHeading(..)] Column does not exist." );
        } else {
            return _columns.at( column )._heading;
        }
    }

    /**
     * Gets the DataType of a column
     * @param column Column number
     * @return The DataType
     */
    inline TableDBCell::DataType TableDB::getColType( const size_t &column ) const {
        if( column >= getColCount() ) {
            LOG_ERROR( "[eadlib::TableDB::getType( ", column, " )] Column does not exist" );
            return TableDBCell::DataType::NONE;
        } else {
            return _columns.at( column )._type;
        }
    }

    /**
     * Gets the number of columns in the table
     * @return The size
     */
    inline size_t TableDB::getColCount() const {
        return _table_size_x;
    }

    /**
     * Gets the number of Rows in the table
     * @return The number of rows
     */
    inline size_t TableDB::getRowCount() const {
        if( _rows.size() == 0 ) {
            return 0;
        }
        return _table_size_y;
    }

    /**
     * Checks that all data (cells) match the column types
     * and all rows and columns are respectively the same size
     * @return Success
     */
    inline bool TableDB::checkIntegrity() {
        if( checkBorders() ) {
            size_t number_of_issues = 0;
            for( size_t i = 0; i < _table_size_x; i++ ) {
                for( size_t j = 0; j < _table_size_y; j++ ) {
                    if( _columns.at( i )._type == TableDBCell::DataType::NONE ) {
                        LOG_WARNING( "[eadlib::TableDB::checkIntegrity()] Column '", i, "' is of undefined type." );
                    } else {
                        if( _rows.at( j ).at( i ).getType() == TableDBCell::DataType::NONE ) {
                            LOG_TRACE( "[eadlib::TableDB::checkIntegrity()] Item of type 'NONE' (null) found." );
                        } else {
                            if( _rows.at( j ).at( i ).getType() != _columns.at( i )._type ) {
                                LOG_ERROR( "[eadlib::TableDB::checkIntegrity()] "
                                               "Item type ('", static_cast<int>( _rows.at( j ).at( i ).getType() ),
                                           "') does not match the column type ('", static_cast<int>( _columns.at( i )._type ), "')." );
                                number_of_issues++;
                            }
                        }
                    }
                }
            }
            if( number_of_issues > 0 ) {
                LOG_ERROR( "[eadlib::TableDB::checkIntegrity()] ", number_of_issues, " Items do not match their column types." );
                return false;
            } else {
                LOG( "[eadlib::TableDB::checkIntegrity()] Items match their respective column types." );
                return true;
            }
        } else {
            return false;
        }
    }

    /**
     * Checks that all rows and columns are respectively the same size
     * @return Success
     */
    inline bool TableDB::checkBorders() {
        LOG_TRACE( "[eadlib::TableDB::checkBorders()] Checking Table borders are consistent." );
        _integrity_flag = false;
        //check data table
        size_t base_height = _rows.size();
        size_t base_width  = _columns.size(); //_rows.at( 0 ).getLength();
        //Column(s) exist
        if( getColCount() < 1 ) {
            LOG_ERROR( "[eadlib::TableDB::checkBorders()] Table has no Columns." );
            return false;
        }
        if( getRowCount() < 1 ) {
            LOG_ERROR( "[eadlib::TableDB::checkBorders()] Table has no Rows." );
            return false;
        }
        //Row width consistency
        for( int i = 0; i < base_height; i++ ) {
            if( _rows.at( i ).getLength() != base_width ) {
                LOG_ERROR( "[eadlib::TableDB::checkBorders()] Row(s) not the same width." );
                return false;
            }
        }
        //Data's row width == number of columns ?
        if( base_width != _columns.size() ) {
            LOG_ERROR( "[eadlib::TableDB::checkBorders()] Data width ('", base_width, "') and Heading width ('", _columns.size(), "') not the same." );
            return false;
        }
        //check vector sizes with internally stored sizes
        if( base_height != _table_size_y ) {
            LOG_ERROR( "[eadlib::TableDB::checkBorders()] Inconsistency in table height stored." );
            return false;
        }
        if( base_width != _table_size_x ) {
            LOG_ERROR( "[eadlib::TableDB::checkBorders()] Inconsistency in table width stored." );
            return false;
        }
        _integrity_flag = true;
        LOG_TRACE( "[eadlib::TableDB::checkBorders()] > All rows are of consistent and correct width." );
        return _integrity_flag;
    }

    /**
     * Searches for a pair of values inside specified column names (good for key pairs)
     * @param column1 Name of the first column
     * @param item1 Value 1
     * @param column2 Name of the second column
     * @param item2 Value 2
     * @return Pair with the row reached with the first match and a find success flag
     */
    inline std::pair<size_t, bool> TableDB::searchFor( const std::string &column1, const TableDBCell &item1,
                                                const std::string &column2, const TableDBCell &item2 ) { //order doesn't matter
        auto col_index1 = findColumn( column1 );
        auto col_index2 = findColumn( column2 );
        if( col_index1.second && col_index2.second ) {
            return findPair( col_index1.first, item1, col_index2.first, item2 );
        } else {
            LOG_ERROR( "[eadlib::TableDB::searchFor( ", column1, ", ", item1, ", ", column2, " )] column name(s) does not match any columns in table." );
            std::make_pair( 0, false );
        }
    }

    //-----------------------------------------------------------------------------------------------------------------
    // TableDB class private method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Creates a new Row in the 'Table' container
     */
    inline void TableDB::createRow() {
        resetIntegrityFlag();
        _rows.push_back( TableDBRow { } );
        _table_size_y++;
    }

    /**
     * Resets the consistency flag for the table
     */
    inline void TableDB::resetIntegrityFlag() {
        _integrity_flag = false;
    }

    /**
     * Calculate the location of the next cell in the table
     * @param current Current cell position
     * @return Next cell Coordinates
     */
    inline TableDBCursor TableDB::getNextCellFor( const TableDBCursor &current ) {
        TableDBCursor next {};
        if( current.isReset() ) {
            next.set( 0, 0 );
        } else if( current.getCol() == getColCount() - 1 ) {
            next.set( current.getRow() + 1 , 0 );
        } else {
            next.set( current.getRow(), current.getCol() + 1 );
        }
        return next;
    }

    /**
     * Checks the next cell after the last entry in the table and if the types matches
     * @param next Cursor position of the next cell
     * @param type Type of the value that we want to add at the end of the table
     * @return Success of the checks
     */
    inline bool TableDB::checkNext( TableDBCursor &next, TableDBCell::DataType type ) {
        if( getColCount() < 1 ) {
            LOG_ERROR( "[eadlib::Table::checkNext] No Column(s) in Table" );
            return false;
        }
        //First entry case
        if( _cursor.isReset() ) {
            createRow();
            _cursor = next;
            if( type != TableDBCell::DataType::NONE ) {
                if( type != _columns.at( _cursor.getCol() )._type ) {
                    LOG_ERROR( "[eadlib::TableDB::checkNext( ", next, ", ", static_cast<int>( type ), " )] "
                        "Type mismatch on first entry, got '", static_cast<int>( _columns.at( next.getCol() )._type ), "'." );
                    return false;
                }
            }
        } else {
            //All subsequent entries case
            if( type != TableDBCell::DataType::NONE ) {
                if( type != _columns.at( next.getCol() )._type ) {
                    LOG_ERROR( "[eadlib::Table::checkNext( ", next, ", ", static_cast<int>( type ), " )] "
                        "Type mismatch, got '", static_cast<int>( _columns.at( next.getCol() )._type ), "." );
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Checks that the last cell position stored matches the last cell of the table
     * @return Success
     */
    inline bool TableDB::checkLastCellPosition() {
        if( ( _cursor.getCol() == _columns.size() - 1 ) &&
            ( _cursor.getRow() == _rows.size() - 1 ) ) {
            return true;
        }
        LOG_ERROR( "[eadlib::TableDB::checkLastCellPosition()] Last cell position not correctly recorded." );
        return false;
    }


    /**
     * Find occurring pair of values in a row
     * @param col1 Column to look for first Item
     * @param value1 First value to look for
     * @param col2 Column to look for second Item
     * @param value2 Second value to look for
     * @return Pair with row number reached where pair occurs and find success flag
     */
    inline std::pair<size_t, bool> TableDB::findPair( const size_t &col1, const TableDBCell &value1, const size_t &col2, const TableDBCell &value2 ) {
        LOG_DEBUG( "[eadlib::TableDB::findpair( ", col1, ", ", value1, ", ", col2, ", ", value2, " )] Looking for pair." );
        for( size_t i = 0; i < _rows.size(); i++ ) {
            if( _rows.at( i ).at( col1 ) == value1 ) {
                if( _rows.at( i ).at( col2 ) == value2 ) {
                    return std::make_pair( i, true );
                }
            }
            if( _rows.at( i ).at( col1 ) == value2 ) {
                if( _rows.at( i ).at( col2 ) == value1 ) {
                    return std::make_pair( i, true );
                }
            }
        }
        return std::make_pair( 0, false );
    }
}

#endif //EADLIB_DBTABLE_H
