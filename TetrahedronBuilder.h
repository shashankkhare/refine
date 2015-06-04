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
    GDALRasterBand* openGDALDS()
    {
        GDALDataset* poDS;
        GDALAllRegister();
        poDS = (GDALDataset*)GDALOpen(datasrc_name.c_str(), GA_ReadOnly);
        if (poDS == NULL) {
            std::cout << "Open failed.\n";
            return NULL;
        }

        double adfGeoTransform[6];
        cols = poDS->GetRasterXSize();
        rows =  poDS->GetRasterYSize();
        int band_count = poDS->GetRasterCount();

        if (poDS->GetGeoTransform(adfGeoTransform) == CE_None) {
        } else {
            // error could not get geotransformation data
            return NULL;
        };
        
        startx = adfGeoTransform[0];
        starty = adfGeoTransform[3];
        resx = adfGeoTransform[1];
        resy = adfGeoTransform[5];

        return poDS->GetRasterBand(1);
    }

public:
    Build_triangle(std::string data_source)
    {
        datasrc_name = data_source;
    }

    void operator()(HDS& hds)
    {
        // open data source
        // by default read the first band
        GDALRasterBand *poBand = openGDALDS();
        if (poBand == NULL) return;
        
        double *pafScanline;
        pafScanline = (double *)CPLMalloc(sizeof(double)*cols);
        
        for (int i = 0; i < rows; i++) {
            poBand->RasterIO(GF_Read, 0, 0, cols, 1, pafScanline, cols, 1, GDT_Float64, 0, 0);
        };

        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
        //max num vertices,faces,half edges needs to be provided
        B.begin_surface(rows*cols, rows*cols / 2, rows*cols * 2);
        for (int i = 0; i < rows; i++) {
            // add three vertices at a time
            std::cout << "adding point" << listp[i] << std::endl << std::flush;
            B.add_vertex(listp[i]);
            if (i >= 2) {

                B.begin_facet();
                if (i % 2 == 0) {
                    B.add_vertex_to_facet(i - 2);
                    B.add_vertex_to_facet(i);
                    B.add_vertex_to_facet(i - 1);
                } else {
                    B.add_vertex_to_facet(i - 2);
                    B.add_vertex_to_facet(i - 1);
                    B.add_vertex_to_facet(i);
                }
                B.end_facet();
            }
        }
        B.end_surface();
    }
};
