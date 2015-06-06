#include <iostream>
#include <fstream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
// Adaptor for Polyhedron_3
#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>
// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>

#include "CommonTypes.h"
#include "TetrahedronBuilder.h"

using namespace std;
namespace SMS = CGAL::Surface_mesh_simplification ;

void writeSTL(Polyhedron& mesh, std::string filename)
{
        CGAL::set_ascii_mode(std::cout);
    ofstream mfile(filename.c_str());
    //mfile.open(filename.c_str());
      mfile<< mesh;    
};

#include "optionparser.h"
enum optionIndex {
    FILENAME,
    HELP,
    PLUS
};
const option::Descriptor usage[] = {
    { FILENAME, 0, "f", "file",option::Arg::Optional , "USAGE: example --file=<filename> \n" },
    { HELP, 0, "", "help", option::Arg::None, "  --help  \tPrint usage and exit." },
    { PLUS, 0, "p", "plus", option::Arg::None, "  --plus, -p  \tIncrement count." },
    { 0, 0, 0, 0, 0, 0 }
};

int main(int argc, char** argv)
{

    argc -= (argc > 0);
    argv += (argc > 0); // skip program name argv[0] if present
    option::Stats stats(usage, argc, argv);
    option::Option* options = new option::Option[stats.options_max];
    option::Option* buffer = new option::Option[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error())
        return 1;

    if (options[HELP] || argc == 0) {
        option::printUsage(std::cout, usage);
        return 0;
    }
    
    if (!options[FILENAME]) {
        std::cout << "Error:elevation file not provided.";
        return -1;
    }
    option::Option* opt = options[FILENAME];
    std::string filename = std::string(opt->arg);
    std::cout << "file option: " << filename << "\n";
    // build the mesh of the terrain from DEM
    Polyhedron mesh;
    TetrahedronBuilder<HalfedgeDS> triangle_builder(filename);
    mesh.delegate(triangle_builder);
    writeSTL(mesh, "basic.off");
 
    //simplify the mesh 
    std::cout << "simplifying mesh ...";
     SMS::Count_stop_predicate<Polyhedron> stop(mesh.size_of_vertices()/2);
       int r = SMS::edge_collapse
            (mesh
            ,stop
            ,CGAL::vertex_index_map(boost::get(CGAL::vertex_external_index,mesh)) 
              .edge_index_map  (boost::get(CGAL::edge_external_index  ,mesh))  
            );
  
    // write out the mesh
    std::cout << "Mesh :::" << std::endl << mesh.size_of_vertices() << ' ' << mesh.size_of_facets() << " 0" << std::endl;
    writeSTL(mesh, "simplified.off");

    //extrude to form volume
    //extrude(P);
    
    return 0;
}
