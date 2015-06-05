#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <iostream>
#include <fstream>
#include <CGAL/IO/Polyhedron_iostream.h>

#include "CommonTypes.h"
#include "TetrahedronBuilder.h"

using namespace std;

void writeSTL(Polyhedron& P, std::string filename)
{
    ofstream myfile;
    myfile.open(filename.c_str());
    myfile << P;
};

#include "optionparser.h"
enum optionIndex {
    FILENAME,
    HELP,
    PLUS
};
const option::Descriptor usage[] = {
    { FILENAME, 0, "", "file", option::Arg::None, "USAGE: example --file=<filename> \n" },
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
    
    if (options[FILENAME]) {
    option::Option* opt = options[FILENAME];
    std::string filename = std::string(opt->name,opt->namelen);
    std::cout << "file option: " << filename << "\n";
    // build the mesh
    Polyhedron P;
    TetrahedronBuilder<HalfedgeDS> triangle_builder(filename);
    P.delegate(triangle_builder);
    
    // write out the mesh
    std::cout << "OFF" << std::endl << P.size_of_vertices() << ' ' << P.size_of_facets() << " 0" << std::endl;
    CGAL::set_ascii_mode(std::cout);

    // for ( Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v)
    //   std::cout << v->point() << std::endl;

    writeSTL(P, "test.off");
    }
    else {
        std::cout << "no file specified";
    }
    return 0;
}
