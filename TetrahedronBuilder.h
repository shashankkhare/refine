#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "CommonTypes.h"
//modifier class to build polyhedron from list of points supplied
template <class HDS>
class Build_triangle : public CGAL::Modifier_base<HDS>
{
private:
    std::vector<Point_3> listp;
    
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
