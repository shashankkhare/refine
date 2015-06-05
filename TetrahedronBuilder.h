#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "CommonTypes.h"
#include <gdal/gdal_priv.h>

/**
 * @section DESCRIPTION
 * modifier class to build polyhedron from list of points supplied
   builds an initial tetrahedron mesh from
   speficied GIS data source (gdal compatible).>
 */
template <class HDS> class Build_triangle : public CGAL::Modifier_base<HDS>
{
private:
    std::vector<Point_3> listp;
    std::string datasrc_name;
    // gis data
    double startx;
    double starty;
    double resx;
    double resy;
    int rows;
    int cols;
    GDALRasterBand* openGDALDS();
	 void Build(HDS &hds);
	 
public:

    Build_triangle(std::string data_source)
    {
        datasrc_name = data_source;
    }

    void operator()(HDS& hds)
	{
		Build(hds);
	}