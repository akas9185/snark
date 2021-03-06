// This file is part of snark, a generic and flexible library for robotics research
// Copyright (c) 2011 The University of Sydney
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the The University of Sydney.
// 4. Neither the name of the The University of Sydney nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
#include <snark/point_cloud/voxel_grid.h>

namespace snark { namespace test {

typedef Eigen::Vector3d point;
typedef Eigen::Matrix< std::size_t, 1, 3 > index_type;
typedef snark::math::closed_interval< double, 3 > extents_type;
    
TEST( voxel_grid, construction )
{
    extents_type e( point( 0, 0, 0 ), point( 10, 10, 10 ) );
    point resolution( 0.2, 0.2, 0.2 );
    snark::voxel_grid< int > g( e, resolution );
    snark::voxel_grid< int > h( g );
    h = g;
}

TEST( voxel_grid, test )
{
    extents_type e( point( 0, 0, 0 ), point( 10, 10, 5 ) );
    snark::voxel_grid< int > grid( e, point( 0.2, 0.2, 0.2 ) );
    
    EXPECT_TRUE( !grid.covers( point( 10.001, 1, 1 ) ) );
    EXPECT_TRUE( !grid.covers( point( 1, 1, 5.001 ) ) );
    EXPECT_TRUE( !grid.covers( point( -0.001, 1, 1 ) ) );
    EXPECT_TRUE( !grid.covers( point( 1, -0.001, 1 ) ) );
    EXPECT_TRUE( !grid.covers( point( 1, 1, -0.001 ) ) );
    
    EXPECT_TRUE( grid.covers( point( 10, 1, 1 ) ) );
    EXPECT_TRUE( grid.covers( point( 1, 1, 5 ) ) );
    EXPECT_TRUE( grid.covers( point( 0, 1, 1 ) ) );
    EXPECT_TRUE( grid.covers( point( 1, 0, 1 ) ) );
    EXPECT_TRUE( grid.covers( point( 1, 1, 0 ) ) );
    EXPECT_TRUE( grid.covers( point( 1, 1, 1 ) )  );

    snark::voxel_grid< int >::index_type index = grid.index_of( point( 1, 1, 1 ) );
    point origin = grid.origin( index );
    EXPECT_EQ( origin[0] , ( 0.2 * double( index[0] ) + 0.0 ) );
    EXPECT_EQ( origin[1] , ( 0.2 * double( index[1] ) + 0.0 ) );
    EXPECT_EQ( origin[2] , ( 0.2 * double( index[2] ) + 0.0 ) );

    for( unsigned int i = 0; i < grid.size()[0]; ++i )
    {
        point p( i, i, i );
        //if( !e.has( p ) ) { EXPECT_TRUE( !grid.covers( p ) ); continue; }
        //EXPECT_TRUE( grid.covers( p ) );
        if( !grid.covers( p ) ) { continue; }
        int* v = grid.touch_at( p );
        EXPECT_TRUE( v );
        *v = 1234;
        index_type ind = grid.index_of( p );
        EXPECT_EQ( *v, grid( ind ) );
        EXPECT_EQ( *v, grid( ind[0], ind[1], ind[2] ) );
        point q = p + point( 0.001, 0.001, 0.001 );
        if( grid.covers( q ) )
        {
            int* w = grid.touch_at( q );
            EXPECT_TRUE( w );
            //EXPECT_EQ( w, v ); todo: invalid test, since the elements may get reallocated; fix... sometime...
            ++( *w );
            grid.erase_at( q );
            EXPECT_TRUE( !grid.exists( grid.index_of( q ) ) );
        }
    }
}

} } // namespace snark {  namespace test {

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

