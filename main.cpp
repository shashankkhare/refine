#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <iostream>
#include <CGAL/Polyhedron_incremental_builder_3.h>

typedef CGAL::Simple_cartesian<double>     Kernel;
typedef Kernel::Point_3                    Point_3;
typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;
typedef Polyhedron::Vertex_iterator        Vertex_iterator;
typedef Polyhedron::Facet_iterator        Facet_iterator;
typedef Polyhedron::Facet        PFacet;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
typedef Polyhedron::HalfedgeDS             HalfedgeDS;

//modifier class to build polyhedron from list of points supplied
template <class HDS>
class Build_triangle : public CGAL::Modifier_base<HDS>
{
private:
    std::vector<Point_3> listp;
    int nrow;
    int ncol;

public:
    Build_triangle(std::vector<Point_3> plist)
    {
        listp = plist;
    }
    void operator()( HDS& hds)
    {
        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        int point_list_size = listp.size();
        B.begin_surface( point_list_size, point_list_size/2, point_list_size*2);
        for (int i=0; i< point_list_size; i++)
        {
            //add three vertices at a time
            std::cout << "adding point"  << listp[i] << std::endl  << std::flush;
            B.add_vertex( listp[i]);
            if (i>=2)
            {

                B.begin_facet();
                if (i%2==0) {
                    B.add_vertex_to_facet( i-2);
                    B.add_vertex_to_facet( i);
                    B.add_vertex_to_facet( i-1);
                }else {
                    B.add_vertex_to_facet( i-2);
                    B.add_vertex_to_facet( i-1);
                    B.add_vertex_to_facet( i);
                }

                B.end_facet();
            }
        }
     B.end_surface();
    }
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

    for ( Facet_iterator f = P.facets_begin(); f != P.facets_end(); ++f)
    {
        std::cout << "-------\n";
        Halfedge_facet_circulator j = f->facet_begin();
        int num_vertices = CGAL::circulator_size(j);
        HalfedgeDS::Vertex v;
        for (int i=0; i<num_vertices; i++)
        {
            v = j->vertex();
            j++;
            std::cout << v.point()<< "\n";
        };
    }
    return 0;
}

