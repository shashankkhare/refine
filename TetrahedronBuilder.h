#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "CommonTypes.h"
#include <gdal/gdal_priv.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>

/**
 * @section DESCRIPTION
 * modifier class to build polyhedron from list of points supplied
   builds an initial tetrahedron mesh from
   speficied GIS data source (gdal compatible).>
 */
template <class HDS> class TetrahedronBuilder : public CGAL::Modifier_base<HDS>
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
        rows = poDS->GetRasterYSize();
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
        std::cout << "startx = " << startx << " starty= " << starty << " resx= " << resx << " resy=" << resy <<"\n";
        return poDS->GetRasterBand(1);
    }
    /**
     * @brief Generates the tetrahedron from gdal datasource
     * @param hds
     */
    void Build(HDS& hds)
    {

        // open data source
        // by default read the first band
        GDALRasterBand* poBand = openGDALDS();
        if (poBand == NULL)
            return;

        double* pafScanline;
        pafScanline = (double*)CPLMalloc(sizeof(double) * cols);
        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
        //max num vertices,faces,half edges needs to be provided
        B.begin_surface(rows*cols, rows*cols / 2, rows*cols * 2);
        
/********** create vertices *******************/
            std::cout << "creating vertices \n" ;
        for (int i = 0; i < rows; i++) {
            poBand->RasterIO(GF_Read, 0, i, cols, 1, pafScanline, cols, 1, GDT_Float64, 0, 0);
            for (int j = 0; j < cols; j++) {
               B.add_vertex(Point_3(startx+j*resx,starty+i*resy,pafScanline[j])) ;    
            }
        };
        
/************ create facets here *****************/
        std::cout << "creating facets \n";
        int first_vid,second_vid,third_vid;
        for (int i = 0; i < rows; i++) 
        {
            if (i != 0)
            for (int j=0;j<cols-1;j++)
            {
            // for each col add two triangles as we 
            // are creating a tetrahedron. 
            // 
                    //add the first facet
                    B.begin_facet();
                    //start adding facets  
                    first_vid = (i-1)*cols+j;
                    second_vid = (i-1)*cols + j +1;
                    third_vid = i*cols+j;
                    B.add_vertex_to_facet(first_vid);
                    B.add_vertex_to_facet(second_vid);
                    B.add_vertex_to_facet(third_vid);
                    B.end_facet();
                    
                    //add the second facet
                    B.begin_facet();
                    first_vid = i*cols+j;
                    second_vid = (i-1)*cols+j+1;
                    third_vid  = i*cols+j +1; 
                    B.add_vertex_to_facet(first_vid);
                    B.add_vertex_to_facet(second_vid);
                    B.add_vertex_to_facet(third_vid);
                    B.end_facet();
            }
        }
        B.end_surface();


    };


public:
TetrahedronBuilder(std::string data_source)
{
    datasrc_name = data_source;
}

void operator()(HDS& hds)
{
    Build(hds);
}
}
;
