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

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <comma/application/command_line_options.h>
#include <comma/base/types.h>
#include <comma/csv/options.h>
#include <comma/csv/stream.h>
#include <comma/name_value/parser.h>
#include <comma/string/string.h>
#include <snark/graphics/applications/view_points/MainWindow.h>
#include <snark/graphics/applications/view_points/Viewer.h>
#include <snark/graphics/applications/view_points/ShapeReader.h>
#include <snark/graphics/applications/view_points/ModelReader.h>
#include <snark/graphics/applications/view_points/TextureReader.h>
#include <QApplication>

void usage()
{
    std::cerr << std::endl;
    std::cerr << "view 3D point clouds:" << std::endl;
    std::cerr << "view points from given files/streams and stdin" << std::endl;
    std::cerr << "(see examples below for a quick start)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "note: scene radius, centre and point of view will be decided" << std::endl;
    std::cerr << "      depending on the extents of the first data source" << std::endl;
    std::cerr << "      (if you want it to be stdin, specify \"-\" as the" << std::endl;
    std::cerr << "      explicitly as the first data source); see also" << std::endl;
    std::cerr << "      --scene-radius option and examples" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: view-points [<options>] [<filenames>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "input data options" << std::endl;
    std::cerr << "    --colour,--color,-c <how>: how to colour points" << std::endl;
    std::cerr << "        <how>:" << std::endl;
    std::cerr << "            colour maps" << std::endl;
    std::cerr << "                <min>:<max>: if scalar field present, stretch by scalar (see section <fields>)" << std::endl;
    std::cerr << "                <min>:<max>,<from-colour>:<to-colour>: if scalar field present, stretch by scalar (see section <fields>)" << std::endl;
    std::cerr << "                <min>:<max>,<colourmap>: if scalar field present, stretch by scalar (see section <fields>)" << std::endl;
    std::cerr << "                    <colourmap>: jet, hot, red, green" << std::endl;
    std::cerr << std::endl;
    std::cerr << "            fixed colour" << std::endl;
    std::cerr << "                white, black, red, green, blue, yellow, cyan, magenta, grey, pink, sky, salad: fixed colour" << std::endl;
    std::cerr << std::endl;
    std::cerr << "            colour by id" << std::endl;
    std::cerr << "                if there is \"id\" field in --fields, colour by id, with or without scalar (see section <fields>)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "            colour by elevation" << std::endl;
    std::cerr << "                [<min>:<max>][,<from-colour>:<to-colour>][,cyclic][,sharp][,quadratic] (in any order)" << std::endl;
    std::cerr << "                    stretch colours by elevation" << std::endl;
    std::cerr << "                    <min>:<max>: from-to in metres, e.g. -3:10" << std::endl;
    std::cerr << "                    <from-colour>:<to-colour>: from which to which colour" << std::endl;
    std::cerr << "                    cyclic: if present, colour cyclically with <min> and <max>" << std::endl;
    std::cerr << "                            meaning the height of the corresponding stip" << std::endl;
    std::cerr << "                    sharp: if present (for cyclic only), do not make smooth borders" << std::endl;
    std::cerr << "                           between the colours" << std::endl;
    std::cerr << "                    quadratic: if present (for cyclic only), parabolic stretching, quick and dirty" << std::endl;
    std::cerr << "                                default: stretch colours linearly" << std::endl;
    std::cerr << "                    e.g.: cat data.csv | view-points --colour=blue:yellow,1:2,cyclic" << std::endl;
    std::cerr << std::endl;
    std::cerr << "            default: stretched by elevation from cyan to magenta from 0:1" << std::endl;
    std::cerr << std::endl;
    std::cerr << "      hide: e.g. \"test.csv;hide\": hide the source, when shown first time (useful, when there are very many inputs" << std::endl;
    std::cerr << "    --label <label>: text label displayed next to the latest point" << std::endl;
    std::cerr << "    --no-stdin: do not read from stdin" << std::endl;
    std::cerr << "    --point-size,--weight <point size>: default: 1" << std::endl;
    std::cerr << "    --shape <shape>: \"point\", \"extents\", \"line\", \"label\"; default \"point\"" << std::endl;
    std::cerr << "                     \"arc\": e.g: --shape=arc --fields=,,begin,end,centre," << std::endl;
    std::cerr << "                               or: --shape=arc --fields=,,begin,middle,end,,," << std::endl;
    std::cerr << "                                   where 'begin' and 'end' are x,y,z points" << std::endl;
    std::cerr << "                                   and 'middle' is a point between begin and end on the arc" << std::endl;
    std::cerr << "                                   default: 'begin,end', with centre 0,0,0" << std::endl;
    std::cerr << "                     \"ellipse\": e.g. --shape=ellipse --fields=,,center,orientation,minor,major," << std::endl;
    std::cerr << "                                  orientation: roll,pitch,yaw; default: in x,y plane" << std::endl;
    std::cerr << "                     \"extents\": e.g. --shape=extents --fields=,,min,max,,," << std::endl;
    std::cerr << "                     \"line\": e.g. --shape=line --fields=,,first,second,,," << std::endl;
    std::cerr << "                     \"lines\": connect all points of a block from first to the last; fields same as for 'point'" << std::endl;
    std::cerr << "                     \"loop\": connect all points of a block; fields same as for 'point'" << std::endl;
    std::cerr << "                     \"label\": e.g. --shape=label --fields=,x,y,z,,,label" << std::endl;
    std::cerr << "                     \"<model file ( obj, ply... )>[;<options>]\": e.g. --shape=vehicle.obj" << std::endl;
    std::cerr << "                     \"    <options>" << std::endl;
    std::cerr << "                     \"        flip\": flip the model around the x-axis" << std::endl;
    std::cerr << "                     \"        scale=<value>\": resize model (ply only, todo), e.g. show model half-size: scale=0.5" << std::endl;
    std::cerr << "                     \"<image file>[,<image options>]:<image file>[,<image options>]\": show image, e.g. --shape=\"vehicle-lights-on.jpg,vehicle-lights-off.jpg\"" << std::endl;
    std::cerr << "                            <image options>: <width>,<height> or <pixel-size>" << std::endl;
    std::cerr << "                                <width>,<height>: image width and height in meters when displaying images in the scene; default: 1,1" << std::endl;
    std::cerr << "                                <pixel size>: single pixel size in metres" << std::endl;
    std::cerr << "                            note 1: just like for the cad models, the images will be pinned to the latest point in the stream" << std::endl;
    std::cerr << "                            note 2: specify id in fields to switch between multiple images, see examples below" << std::endl;
    std::cerr << "    --size <size> : render last <size> points (or other shapes)" << std::endl;
    std::cerr << "                    default 2000000 for points, for 200000 for other shapes" << std::endl;
    std::cerr << std::endl;
    std::cerr << "camera options" << std::endl;
    std::cerr << "    --camera=\"<options>\"" << std::endl;
    std::cerr << "          <options>: [<fov>];[<type>]" << std::endl;
    std::cerr << "          <fov>: field of view in degrees, default 45 degrees" << std::endl;
    std::cerr << "          <type>: orthographic | perspective" << std::endl;
    std::cerr << "              default: perspective" << std::endl;
    std::cerr << "    --fov=<fov>: set camera field of view in degrees" << std::endl;
    std::cerr << "    --camera-config=<filename>: camera config in json; to see an example, run --output-camera-config" << std::endl;
    std::cerr << "    --camera-position=\"<options>\"" << std::endl;
    std::cerr << "          <options>: <position>|<stream>" << std::endl;
    std::cerr << "          <position>: <x>,<y>,<z>,<roll>,<pitch>,<yaw>" << std::endl;
    std::cerr << "          <stream>: position csv stream with options; default fields: x,y,z,roll,pitch,yaw" << std::endl;
    std::cerr << "    --orthographic: use orthographic projection instead of perspective" << std::endl;
    std::cerr << std::endl;
    std::cerr << "more options" << std::endl;
    std::cerr << "    --background-colour <colour> : default: black" << std::endl;
    std::cerr << "    --output-camera-config,--output-camera: output camera position as t,x,y,z,r,p,y to stdout" << std::endl;
    std::cerr << "    --scene-center,--center=<value>: fixed scene center as \"x,y,z\"" << std::endl;
    std::cerr << "    --scene-radius,--radius=<value>: fixed scene radius in metres, since sometimes it is hard to imply" << std::endl;
    std::cerr << "                            scene size from the dataset (e.g. for streams)" << std::endl;
    std::cerr << "    --z-is-up : z-axis is pointing up, default: pointing down ( north-east-down system )" << std::endl;
    std::cerr << std::endl;
    std::cerr << "csv options" << std::endl;
    std::cerr << comma::csv::options::usage() << std::endl;
    std::cerr << std::endl;
    std::cerr << "    fields:" << std::endl;
    std::cerr << "        default: x,y,z" << std::endl;
    std::cerr << "        x,y,z: coordinates (%d in binary)" << std::endl;
    std::cerr << "        id: if present, colour by id (%ui in binary)" << std::endl;
    std::cerr << "        block: if present, clear screen once block id changes (%ui in binary)" << std::endl;
    std::cerr << "        r,g,b: if present, specify RGB colour (0-255; %uc in binary)" << std::endl;
    std::cerr << "        a: if present, specifies colour transparency (0-255, %uc in binary); default 255" << std::endl;
    std::cerr << "        scalar: if present, colour by scalar" << std::endl;
    std::cerr << "                  use --colour=<from>:<to>[,<from colour>:<to colour>]" << std::endl;
    std::cerr << "                  default: 0:1,red:blue" << std::endl;
    std::cerr << "                  todo: implement for shapes (currently works only for points)" << std::endl;
    std::cerr << "        label: text label (currenly implemented for ascii only)" << std::endl;
    std::cerr << "        roll,pitch,yaw: if present, show orientation" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    most of the options can be set for individual files (see examples)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples" << std::endl;
    std::cerr << std::endl;
    std::cerr << "basics" << std::endl;
    std::cerr << "    view points from file:" << std::endl;
    std::cerr << "        view-points xyz.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    hint that the file contains not more than 200000 points" << std::endl;
    std::cerr << "        cat $(ls *.csv) | view-points --size=200000" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    view points from all the binary files in the directory" << std::endl;
    std::cerr << "        cat $(ls *.bin) | view-points --size=200000 --binary \"%d%d%d\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    colour points" << std::endl;
    std::cerr << "        view-points --colour blue $(ls labeled.*.csv)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    each point has an individual color:" << std::endl;
    std::cerr << "        cat xyzrgb.csv | view-points --fields=\"x,y,z,r,g,b\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    view multiple files" << std::endl;
    std::cerr << "        view-points \"raw.csv;colour=0:20\" \"partitioned.csv;fields=x,y,z,id\";point-size=2" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    view a cad model" << std::endl;
    std::cerr << "        echo \"0,0,0\" | view-points --shape /usr/local/etc/segway.shrimp.obj --z-is-up --orthographic" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    use stdin as the primary source for scene radius:" << std::endl;
    std::cerr << "        cat xyz.csv | view-points \"-\" scan.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    specify fixed scene radius explicitly:" << std::endl;
    std::cerr << "        cat xyz.csv | view-points --scene-radius=100" << std::endl;
    std::cerr << std::endl;
    std::cerr << "using images" << std::endl;
    std::cerr << "    show image with given position" << std::endl;
    std::cerr << "        echo 0,0,0 | view-points \"-;shape=image.jpg\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    show resized image" << std::endl;
    std::cerr << "        echo 0,0,0 | view-points \"-;shape=image.jpg,3,4\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    specify pixel size instead of image size" << std::endl;
    std::cerr << "        echo 0,0,0 | view-points \"-;shape=image.jpg,0.1\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    show image with given position and orientation" << std::endl;
    std::cerr << "        echo 0,0,0,0,0,0 | view-points \"-;shape=image.jpg;fields=x,y,z,roll,pitch,yaw\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    switch between images by their index" << std::endl;
    std::cerr << "        echo 0,0,0,0 > points.csv" << std::endl;
    std::cerr << "        echo 0,0,0,1 >> points.csv" << std::endl;
    std::cerr << "        echo 0,0,0,2 >> points.csv" << std::endl;
    std::cerr << "        echo points.csv | view-points \"-;shape=image1.jpg,image2.jpg,image3.jpg;fields=x,y,z,id\"" << std::endl;
    exit( -1 );
}

struct model_options
{
    std::string filename;
    bool flip;
    double scale;
    model_options() : flip( false ), scale( 1.0 ) {}
};

namespace comma { namespace visiting {

template <> struct traits< model_options >
{
    template < typename Key, class Visitor > static void visit( Key, model_options& p, Visitor& v )
    {
        v.apply( "filename", p.filename );
        v.apply( "flip", p.flip );
        v.apply( "scale", p.scale );
    }

    template < typename Key, class Visitor > static void visit( Key, const model_options& p, Visitor& v )
    {
        v.apply( "filename", p.filename );
        v.apply( "flip", p.flip );
        v.apply( "scale", p.scale );
    }
};

} } // namespace comma { namespace visiting {

// quick and dirty, todo: a proper structure, as well as a visitor for command line options
boost::shared_ptr< snark::graphics::View::Reader > makeReader( QGLView& viewer
                                                             , const comma::command_line_options& options
                                                             , const comma::csv::options& csvOptions
                                                             , const std::string& properties = "" )
{
    QColor4ub backgroundcolour( QColor( QString( options.value< std::string >( "--background-colour", "#000000" ).c_str() ) ) );
    comma::csv::options csv = csvOptions;
    std::string shape = options.value< std::string >( "--shape", "point" );
    std::size_t size = options.value< std::size_t >( "--size", shape == "point" ? 2000000 : 200000 );
    unsigned int pointSize = options.value( "--point-size,--weight", 1u );
    std::string colour = options.exists( "--colour" ) ? options.value< std::string >( "--colour" ) : options.value< std::string >( "-c", "-10:10" );
    std::string label = options.value< std::string >( "--label", "" );
    bool show = true;
    if( properties != "" )
    {
        comma::name_value::parser nameValue( "filename", ';', '=', false );
        csv = nameValue.get( properties, csvOptions );
        comma::name_value::map m( properties, "filename", ';', '=' );
        size = m.value( "size", size );
        pointSize = m.value( "point-size", pointSize );
        pointSize = m.value( "weight", pointSize );
        shape = m.value( "shape", shape );
        if( m.exists( "colour" ) ) { colour = m.value( "colour", colour ); }
        else if( m.exists( "color" ) ) { colour = m.value( "color", colour ); }
        label = m.value( "label", label );
        show = !m.exists( "hide" );
    }
    if( !show ) { std::cerr << "view-points: " << csv.filename << " will be hidden on startup; tick the box next to filename to make it visible" << std::endl; }
    snark::graphics::View::coloured* coloured = snark::graphics::View::colourFromString( colour, csv.fields, backgroundcolour );
    if( shape == "point" )
    {
        if( csv.fields == "" ) { csv.fields="x,y,z"; }
        std::vector< std::string > v = comma::split( csv.fields, ',' );
        bool has_orientation = false;
        for( unsigned int i = 0; !has_orientation && i < v.size(); ++i ) { has_orientation = v[i] == "roll" || v[i] == "pitch" || v[i] == "yaw"; }
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< Eigen::Vector3d >( viewer, csv, size, coloured, pointSize, label ) );
        reader->show( show );
        return reader;
    }
    if( shape == "loop" )
    {
        if( csv.fields == "" ) { csv.fields="x,y,z"; }
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< Eigen::Vector3d, snark::graphics::View::how_t::loop >( viewer, csv, size, coloured, pointSize, label ) );
        reader->show( show );
        return reader;
    }
    if( shape == "lines" ) // todo: get a better name
    {
        if( csv.fields == "" ) { csv.fields="x,y,z"; }
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< Eigen::Vector3d, snark::graphics::View::how_t::connected >( viewer, csv, size, coloured, pointSize, label ) );
        reader->show( show );
        return reader;
    }
    if( shape == "label" )
    {
        if( csv.fields == "" ) { csv.fields="x,y,z,label"; }
        // TODO
    }
    else if( shape == "ellipse" )
    {
        if( csv.fields == "" ) { csv.fields="center,orientation,major,minor"; }
        std::vector< std::string > v = comma::split( csv.fields, ',' );
        for( std::size_t i = 0; i < v.size(); ++i )
        {
            if( v[i] == "x" || v[i] == "y" || v[i] == "z" ) { v[i] = "center/" + v[i]; }
            else if( v[i] == "roll" || v[i] == "pitch" || v[i] == "yaw" ) { v[i] = "orientation/" + v[i]; }
        }
        csv.fields = comma::join( v, ',' );
    }
    else if( shape == "arc" )
    {
        if( csv.fields == "" ) { csv.fields="begin,end"; }
        std::vector< std::string > v = comma::split( csv.fields, ',' );
    }
    else if( shape == "extents" )
    {
        if( csv.fields == "" ) { csv.fields="min,max"; }
    }
    else if( shape == "line" )
    {
        if( csv.fields == "" ) { csv.fields="first,second"; }
    }
    else
    {
        if( csv.fields == "" ) { csv.fields="point,orientation"; csv.full_xpath = true; }
        std::vector< snark::graphics::View::TextureReader::image_options > image_options;
        std::vector< std::string > v = comma::split( shape, ':' );
        for( unsigned int i = 0; i < v.size(); ++i )
        {
            std::vector< std::string > w = comma::split( v[i], ',' );
            std::string e = comma::split( w[0], '.' ).back();
            if( e != "png" && e != "jpg" && e != "jpeg" && e != "bmp" && e != "gif" ) { break; }
            switch( w.size() )
            {
                case 1: image_options.push_back( snark::graphics::View::TextureReader::image_options( w[0] ) ); break;
                case 2: image_options.push_back( snark::graphics::View::TextureReader::image_options( w[0], boost::lexical_cast< double >( w[1] ) ) ); break;
                case 3: image_options.push_back( snark::graphics::View::TextureReader::image_options( w[0], boost::lexical_cast< double >( w[1] ), boost::lexical_cast< double >( w[2] ) ) ); break;
                default: COMMA_THROW( comma::exception, "expected <image>[,<width>,<height>]; got: " << shape );
            }
        }
        if( image_options.empty() )
        {
            model_options m = comma::name_value::parser( ';', '=' ).get< model_options >( properties );
            m.filename = shape;
            if( !boost::filesystem::exists( m.filename ) ) { COMMA_THROW( comma::exception, "file does not exist: " << m.filename ); }
            boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ModelReader( viewer, csv, shape, m.flip, m.scale, coloured, label ) );
            reader->show( show );
            return reader;
        }
        else
        {
            boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::TextureReader( viewer, csv, image_options ) );
            reader->show( show );
            return reader;
        }
    }
    std::vector< std::string > v = comma::split( csv.fields, ',' );
    for( std::size_t i = 0; i < v.size(); ++i )
    {
        if(    v[i] != "id"
            && v[i] != "block"
            && v[i] != "colour"
            && v[i] != "label"
            && v[i] != "scalar"
            && v[i] != "" ) { v[i] = "shape/" + v[i]; } }
    csv.fields = comma::join( v, ',' );
    csv.full_xpath = true;
    if( shape == "extents" )
    {
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< snark::math::closed_interval< double, 3 > >( viewer, csv, size, coloured, pointSize, label, snark::math::closed_interval< double, 3 >( Eigen::Vector3d( 0, 0, 0 ), Eigen::Vector3d( 0, 0, 0 ) ) ) );
        reader->show( show );
        return reader;
    }
    else if( shape == "line" )
    {
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< std::pair< Eigen::Vector3d, Eigen::Vector3d > >( viewer, csv, size, coloured, pointSize, label ) );
        reader->show( show );
        return reader;
    }
    else if( shape == "ellipse" )
    {
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< snark::graphics::View::Ellipse< 25 > >( viewer, csv, size, coloured, pointSize, label ) );
        reader->show( show );
        return reader;
    }
    else if( shape == "arc" )
    {
        snark::graphics::View::arc< 20 > sample; // quick and dirty
        if( csv.has_field( "middle" ) || csv.has_field( "middle/x" ) || csv.has_field( "middle/y" ) || csv.has_field( "middle/z" ) ) { sample.middle = Eigen::Vector3d(); }
        boost::shared_ptr< snark::graphics::View::Reader > reader( new snark::graphics::View::ShapeReader< snark::graphics::View::arc< 20 > >( viewer, csv, size, coloured, pointSize, label, sample ) );
        reader->show( show );
        return reader;
    }
    COMMA_THROW( comma::exception, "expected shape, got \"" << shape << "\"" ); // never here
}

int main( int argc, char** argv )
{
    try
    {
        comma::command_line_options options( argc, argv );
        if( options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        comma::csv::options csvOptions( argc, argv );
        std::vector< std::string > properties = options.unnamed( "--z-is-up,--orthographic,--no-stdin,--output-camera-config,--output-camera"
                , "--binary,--bin,-b,--fields,--size,--delimiter,-d,--colour,-c,--point-size,--weight,--background-colour,--scene-center,--center,--scene-radius,--radius,--shape,--label,--camera,--camera-position,--camera-config,--fov,--model,--full-xpath" );
        QColor4ub backgroundcolour( QColor( QString( options.value< std::string >( "--background-colour", "#000000" ).c_str() ) ) );
        boost::optional< comma::csv::options > camera_csv;
        boost::optional< Eigen::Vector3d > cameraposition;
        boost::optional< Eigen::Vector3d > cameraorientation;

        bool camera_orthographic = options.exists( "--orthographic" );
        double fieldOfView = options.value< double >( "--fov" , 45 );
        if( options.exists( "--camera" ) )
        {
            std::string camera = options.value< std::string >( "--camera" );
            std::vector< std::string > v = comma::split( camera, ';' );
            for( std::size_t i = 0; i < v.size(); ++i )
            {
                if( v[i] == "orthographic" )
                {
                    camera_orthographic = true;
                }
                else if( v[i] == "perspective" )
                {
                    camera_orthographic = false;
                }
                else
                {
                    std::vector< std::string > vec = comma::split( v[i], '=' );
                    if( vec.size() == 2 && vec[0] == "fov" )
                    {
                        fieldOfView = boost::lexical_cast< double >( vec[1] );
                    }
                }
            }
        }

        QApplication application( argc, argv );
        bool z_up = options.exists( "--z-is-up" );
        if( options.exists( "--camera-position" ) )
        {
            std::string position = options.value< std::string >( "--camera-position" );
            comma::name_value::parser parser( "x,y,z,roll,pitch,yaw", ',', '=', false );
            snark::graphics::View::point_with_orientation pose;
            try
            {
                pose = parser.get< snark::graphics::View::point_with_orientation >( position );
                cameraposition = pose.point;
                cameraorientation = pose.orientation;
            }
            catch( ... ) {}
            if( !cameraposition )
            {
                comma::name_value::parser parser( "filename", ';', '=', false );
                try
                {
                    std::cerr << " parse " << position << std::endl;
                    camera_csv = parser.get< comma::csv::options >( position );
                    camera_csv->full_xpath = false;
                    if( camera_csv->fields.empty() ) { camera_csv->fields = "x,y,z,roll,pitch,yaw"; }
                }
                catch( ... ) {}
            }
        }
        boost::optional< double > scene_radius = options.optional< double >( "--scene-radius,--radius" );
        boost::optional< Eigen::Vector3d > scene_center;
        boost::optional< std::string > s = options.optional< std::string >( "--scene-center,--center" );
        if( s ) { scene_center = comma::csv::ascii< Eigen::Vector3d >( "x,y,z", ',' ).get( *s ); }
        boost::property_tree::ptree camera_config; // quick and dirty
        if( options.exists( "--camera-config" ) ) { boost::property_tree::read_json( options.value< std::string >( "--camera-config" ), camera_config ); }
        snark::graphics::View::Viewer* viewer = new snark::graphics::View::Viewer( backgroundcolour
                                                                                 , fieldOfView
                                                                                 , z_up
                                                                                 , camera_orthographic
                                                                                 , camera_csv
                                                                                 , cameraposition
                                                                                 , cameraorientation
                                                                                 , options.exists( "--camera-config" ) ? &camera_config : NULL
                                                                                 , scene_center
                                                                                 , scene_radius
                                                                                 , options.exists( "--output-camera-config,--output-camera" ) );
        bool stdinAdded = false;
        for( unsigned int i = 0; i < properties.size(); ++i )
        {
            if( comma::split( properties[i], ';' )[0] == "-" ) { stdinAdded = true; }
            viewer->readers.push_back( makeReader( *viewer, options, csvOptions, properties[i] ) );
        }
        if( !stdinAdded && !options.exists( "--no-stdin" ) )
        {
            csvOptions.filename = "-";
            viewer->readers.push_back( makeReader( *viewer, options, csvOptions ) );
        }
        snark::graphics::View::MainWindow mainWindow( comma::join( argv, argc, ' ' ), viewer );
        mainWindow.show();
        /*return*/ application.exec();
        delete viewer;
    }
    catch( std::exception& ex )
    {
        std::cerr << "view-points: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "view-points: unknown exception" << std::endl;
    }
}
