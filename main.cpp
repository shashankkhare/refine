#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <iostream>
#include <fstream>
#include <CGAL/IO/Polyhedron_iostream.h>

#include "CommonTypes.h"
#include "TetrahedronBuilder.h"

using namespace std;

void writeSTL(Polyhedron &P,std::string filename) {
    ofstream myfile;
    myfile.open(filename.c_str());
    myfile << P;
};

int main()
{
    Point_3 p( 1.0, 0.0, 0.0);
    Point_3 q( 0.0, 1.0, 0.0);
    Point_3 r( 0.0, 0.0, 1.0);
    Point_3 s( 0.0, 0.0, 0.0);
    std::vector < Point_3> plist;
    plist.push_back(p);
    plist.push_back(q);
    plist.push_back(r);
    plist.push_back(s);

    Polyhedron P;
    PFacet f;
    Build_triangle<HalfedgeDS> triangle_builder(plist);
    P.delegate(triangle_builder);

    std::cout << "OFF" << std::endl << P.size_of_vertices() << ' '
              << P.size_of_facets() << " 0" << std::endl;
    CGAL::set_ascii_mode( std::cout);

    //for ( Vertex_iterator v = P.vertices_begin(); v != P.vertices_end(); ++v)
    //   std::cout << v->point() << std::endl;

   writeSTL(P,"test.off");
   
    return 0;
}

