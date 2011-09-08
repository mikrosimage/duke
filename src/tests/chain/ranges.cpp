/*
 * main.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include <RangeImpl.h>

#include <iostream>
#include <vector>
#include <cassert>

#define BOOST_TEST_MODULE RangeTest
#include <boost/test/unit_test.hpp>

using namespace std;

template<typename T>
void displayRange(const ForwardRange<T>& range) {
    auto_ptr<ForwardRange<T> > pCopy(range.save());
    while (!pCopy->empty()) {
        cout << pCopy->front() << " ";
        pCopy->popFront();
    }
    cout << endl;
}

BOOST_AUTO_TEST_SUITE( RangeSuite )

BOOST_AUTO_TEST_CASE( simpleIterator )
{
    const uint64_t values[]= {0,1,2,3,4,5,6};
    const size_t count = sizeof(values)/sizeof(uint64_t);
    SimpleIndexRange<> itr(0,count);
    for(unsigned i=0;i<count;++i, itr.popFront())
    BOOST_CHECK_EQUAL( itr.front(), values[i] );
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( reverseIterator )
{
    const uint64_t values[]= {6,5,4,3,2,1,0};
    const size_t count = sizeof(values)/sizeof(uint64_t);
    SimpleIndexRange<> simpleItr(0,count);
    RetroRange<> itr( simpleItr );
    for(unsigned i=0;i<count;++i, itr.popFront())
    BOOST_CHECK_EQUAL( itr.front(), values[i] );
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( moduloIterator )
{
    const size_t count = 7;
    { // larger does not change anything
        SimpleIndexRange<> simpleItr(0,count);
        ModuloIndexRange<> itr( simpleItr,0,count+1 );
        for(unsigned i=0;i<count;++i, itr.popFront())
        BOOST_CHECK_EQUAL( itr.front(), i );
        BOOST_CHECK( itr.empty() );
    }
    { // all values are between 0 and 1
        SimpleIndexRange<> simpleItr(0,count);
        ModuloIndexRange<> itr( simpleItr, 0, 1);
        for(unsigned i=0;i<count;++i, itr.popFront())
        BOOST_CHECK_EQUAL( itr.front(), i%2 ? 1U : 0U );
        BOOST_CHECK( itr.empty() );
    }
    { // all values are between 2 and 3
        SimpleIndexRange<> simpleItr(0,count);
        ModuloIndexRange<> itr( simpleItr, 2, 3);
        for(unsigned i=0;i<count;++i, itr.popFront())
        BOOST_CHECK_EQUAL( itr.front(), i%2 ? 3U : 2U );
        BOOST_CHECK( itr.empty() );
    }
    displayRange<>( SimpleIndexRange<>(0,10) );
    displayRange<>( RetroRange<>( SimpleIndexRange<>(0,10) ) );
    displayRange<>( ModuloIndexRange<>( SimpleIndexRange<>(0,10) , 5, 9 ) );
    displayRange<>( ModuloIndexRange<>( RetroRange<>( SimpleIndexRange<>(0,10) ), 5, 9 ) );
    displayRange<>( OffsetRange<>( SimpleIndexRange<>(0,10) , 10 ) );
}

BOOST_AUTO_TEST_CASE( alternatingIterator )
{
    const ptrdiff_t values[]= {0,1,-1,2,-2,3,-3,4,-4,5,-5};
    const size_t count = sizeof(values)/sizeof(ptrdiff_t);
    BalancingIndexRange itr(0,count, 0);
    for(unsigned i=0;i<count;++i, itr.popFront())
    BOOST_CHECK_EQUAL( itr.front(), values[i] );
    BOOST_CHECK( itr.empty() );
}

BOOST_AUTO_TEST_CASE( boundAlternatingIterator )
{
    {
        const ptrdiff_t values[]= {0,1,2,3,4,5,6,7,8,9};
        const size_t count = sizeof(values)/sizeof(ptrdiff_t);
        BalancingIndexRange itr(0,count, -1);
        for(unsigned i=0;i<count;++i, itr.popFront())
        BOOST_CHECK_EQUAL( itr.front(), values[i] );
        BOOST_CHECK( itr.empty() );
    }
    {
        const ptrdiff_t values[]= {0,-1,-2,-3,-4,-5,-6,-7,-8,-9};
        const size_t count = sizeof(values)/sizeof(ptrdiff_t);
        BalancingIndexRange itr(0,count, 1);
        for(unsigned i=0;i<count;++i, itr.popFront())
        BOOST_CHECK_EQUAL( itr.front(), values[i] );
        BOOST_CHECK( itr.empty() );
    }
}

BOOST_AUTO_TEST_CASE( playlistIterator0 )
{
    auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(1,0,0, false));
    BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
    m_pRange->popFront();
    BOOST_CHECK( m_pRange->empty() );
}

BOOST_AUTO_TEST_CASE( playlistIterator1 )
{
    auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(3,0,0, false));

    BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
    m_pRange->popFront();
    BOOST_CHECK( m_pRange->empty() );
}

BOOST_AUTO_TEST_CASE( playlistIterator2 )
{
    BOOST_CHECK_THROW( getPlaylistRange(3,3,0, false), runtime_error );
    auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(3,2,0, false));

    BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
    m_pRange->popFront();
    BOOST_CHECK( m_pRange->empty() );
}

BOOST_AUTO_TEST_CASE( playlistIterator3 )
{
    auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(5,2,0, false));

    BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 3 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 4 );
    m_pRange->popFront();
    BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
    m_pRange->popFront();
    BOOST_CHECK( m_pRange->empty() );
}

BOOST_AUTO_TEST_CASE( playlistIterator4 )
{
    { // forward
        auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(5,2,-1, false));

        BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 3 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 4 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
        m_pRange->popFront();
        BOOST_CHECK( m_pRange->empty() );
    }
    { // reverse
        auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(5,2,-1, true));

        BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 4 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 3 );
        m_pRange->popFront();
        BOOST_CHECK( m_pRange->empty() );
    }
}

BOOST_AUTO_TEST_CASE( playlistIterator5 )
{
    { // forward
        auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(5,2,1, false));

        BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 4 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 3 );
        m_pRange->popFront();
        BOOST_CHECK( m_pRange->empty() );
    }
    { // reverse
        auto_ptr<ForwardRange<ptrdiff_t> > m_pRange( getPlaylistRange(5,2,1, true));

        BOOST_CHECK_EQUAL( m_pRange->front(), 2 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 3 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 4 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 0 );
        m_pRange->popFront();
        BOOST_CHECK_EQUAL( m_pRange->front(), 1 );
        m_pRange->popFront();
        BOOST_CHECK( m_pRange->empty() );
    }
}
BOOST_AUTO_TEST_SUITE_END()

