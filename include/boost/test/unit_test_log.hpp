//  (C) Copyright Gennadiy Rozental 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
/// @file
/// @brief defines singleton class unit_test_log and all manipulators.
/// unit_test_log has output stream like interface. It's implementation is
/// completely hidden with pimple idiom
// ***************************************************************************

#ifndef BOOST_TEST_UNIT_TEST_LOG_HPP_071894GER
#define BOOST_TEST_UNIT_TEST_LOG_HPP_071894GER

// Boost.Test
#include <boost/test/tree/observer.hpp>

#include <boost/test/detail/global_typedef.hpp>
#include <boost/test/detail/log_level.hpp>
#include <boost/test/detail/fwd_decl.hpp>

#include <boost/test/utils/wrap_stringstream.hpp>
#include <boost/test/utils/trivial_singleton.hpp>
#include <boost/test/utils/lazy_ostream.hpp>

// Boost

// STL
#include <iosfwd>   // for std::ostream&

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace boost {
namespace unit_test {

// ************************************************************************** //
// **************                log manipulators              ************** //
// ************************************************************************** //

namespace log {

struct BOOST_TEST_DECL begin {
    begin( const_string fn, std::size_t ln )
    : m_file_name( fn )
    , m_line_num( ln )
    {}

    const_string m_file_name;
    std::size_t m_line_num;
};

struct end {};

} // namespace log

// ************************************************************************** //
// **************             entry_value_collector            ************** //
// ************************************************************************** //

namespace ut_detail {

class BOOST_TEST_DECL entry_value_collector {
public:
    // Constructors
    entry_value_collector() : m_last( true ) {}
    entry_value_collector( entry_value_collector const& rhs ) : m_last( true ) { rhs.m_last = false; }
    ~entry_value_collector();

    // collection interface
    entry_value_collector const& operator<<( lazy_ostream const& ) const;
    entry_value_collector const& operator<<( const_string ) const;

private:
    // Data members
    mutable bool    m_last;
};

} // namespace ut_detail

// ************************************************************************** //
// **************                 unit_test_log                ************** //
// ************************************************************************** //

class BOOST_TEST_DECL unit_test_log_t : public test_observer, public singleton<unit_test_log_t> {
public:
    // test_observer interface implementation
    virtual void        test_start( counter_t test_cases_amount );
    virtual void        test_finish();
    virtual void        test_aborted();

    virtual void        test_unit_start( test_unit const& );
    virtual void        test_unit_finish( test_unit const&, unsigned long elapsed );
    virtual void        test_unit_skipped( test_unit const&, const_string );
    virtual void        test_unit_aborted( test_unit const& );

    virtual void        exception_caught( execution_exception const& ex );

    virtual int         priority() { return 1; }

    // log configuration methods
    void                set_stream( std::ostream& );
    void                set_stream( output_format, std::ostream& );
    void                set_threshold_level( log_level );
    void                set_threshold_level( output_format, log_level );

    //! Add a format to the set of loggers
    void                add_format( output_format );

    //! Sets the unique format of the logger
    void                set_format( output_format );


    //! Returns the logger for the specified format.
    unit_test_log_formatter* get_formatter( output_format );

    //! Sets the logger
    //! The specified logger becomes the unique one.
    //! @note the ownership of the pointer is transfered to this instance.
    void                set_formatter( unit_test_log_formatter* );

    // test progress logging
    void                set_checkpoint( const_string file, std::size_t line_num, const_string msg = const_string() );

    // entry logging
    unit_test_log_t&    operator<<( log::begin const& );        // begin entry
    unit_test_log_t&    operator<<( log::end const& );          // end entry
    unit_test_log_t&    operator<<( log_level );                // set entry level
    unit_test_log_t&    operator<<( const_string );             // log entry value
    unit_test_log_t&    operator<<( lazy_ostream const& );      // log entry value

    ut_detail::entry_value_collector operator()( log_level );   // initiate entry collection

private:
    // Implementation helpers
    bool                log_entry_start(output_format log_format);
    void                log_entry_context( log_level l );
    void                clear_entry_context();

    BOOST_TEST_SINGLETON_CONS( unit_test_log_t )
}; // unit_test_log_t

BOOST_TEST_SINGLETON_INST( unit_test_log )

// helper macros
#define BOOST_TEST_LOG_ENTRY( ll )                                                  \
    (::boost::unit_test::unit_test_log                                              \
        << ::boost::unit_test::log::begin( BOOST_TEST_L(__FILE__), __LINE__ ))(ll)  \
/**/

} // namespace unit_test
} // namespace boost

// ************************************************************************** //
// **************       Unit test log interface helpers        ************** //
// ************************************************************************** //

#define BOOST_TEST_MESSAGE( M )                                 \
    BOOST_TEST_LOG_ENTRY( ::boost::unit_test::log_messages )    \
    << BOOST_TEST_LAZY_MSG( M )                                 \
/**/

//____________________________________________________________________________//

#define BOOST_TEST_PASSPOINT()                                  \
    ::boost::unit_test::unit_test_log.set_checkpoint(           \
        BOOST_TEST_L(__FILE__),                                 \
        static_cast<std::size_t>(__LINE__) )                    \
/**/

//____________________________________________________________________________//

#define BOOST_TEST_CHECKPOINT( M )                              \
    ::boost::unit_test::unit_test_log.set_checkpoint(           \
        BOOST_TEST_L(__FILE__),                                 \
        static_cast<std::size_t>(__LINE__),                     \
        (::boost::wrap_stringstream().ref() << M).str() )       \
/**/

//____________________________________________________________________________//

#include <boost/test/detail/enable_warnings.hpp>

#endif // BOOST_TEST_UNIT_TEST_LOG_HPP_071894GER

