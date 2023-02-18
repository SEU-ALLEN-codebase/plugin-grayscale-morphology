#ifndef MED_FILTER_H
#define MED_FILTER_H

#include <v3d_interface.h>
#include <QVector3D>

template <class T> void median_filter(T* data1d,
                                      T* outimg,
                                      V3DLONG in_sz[4],
                                      QVector3D radius, V3DLONG order)
{

    auto N = in_sz[0], M = in_sz[1], P = in_sz[2];
    auto&& pagesz = N*M*P;

    V3DLONG Wx = radius[0], Wy = radius[1], Wz = radius[2];

    T tmp;
    int ii,jj;
    int size = (2*Wx+1)*(2*Wy+1)*(2*Wz+1);
    QSharedPointer<T> arr_buffer(new T[size], std::default_delete<T[]>());
    if (arr_buffer.isNull())
        throw "Median Filter: Memory allocation for temp array.";
    auto&& arr = arr_buffer.data();

    //Median Filtering
    for (V3DLONG iz = 0; iz < P; iz++)
    {
        printf("\r median filter : %d %% completed ", ((iz + 1)*100) / P); fflush(stdout);
        V3DLONG offsetk = iz*M*N;
        for (V3DLONG iy = 0; iy < M; iy++)
        {
            V3DLONG offsetj = iy*N;
            for (V3DLONG ix = 0; ix < N; ix++)
            {

                V3DLONG xb = ix-Wx; if (xb < 0) xb = 0;
                V3DLONG xe = ix+Wx; if (xe >= N-1) xe = N-1;
                V3DLONG yb = iy-Wy; if (yb < 0) yb = 0;
                V3DLONG ye = iy+Wy; if (ye >= M-1) ye = M-1;
                V3DLONG zb = iz-Wz; if (zb < 0) zb = 0;
                V3DLONG ze = iz+Wz; if (ze >= P-1) ze = P-1;

                ii = 0;

                for(V3DLONG k=zb; k<=ze; k++)
                {
                    V3DLONG offsetkl = k*M*N;
                    for(V3DLONG j=yb; j<=ye; j++)
                    {
                        V3DLONG offsetjl = j*N;
                        for(V3DLONG i=xb; i<=xe; i++)
                        {
                            T dataval = data1d[offsetkl + offsetjl + i];
                            arr[ii] = dataval;
                            if (ii>0)
                            {
                                jj = ii;
                                while(jj > 0 && arr[jj-1]>arr[jj])
                                {
                                    tmp = arr[jj];
                                    arr[jj] = arr[jj-1];
                                    arr[jj-1] = tmp;
                                    jj--;
                                }
                            }
                            ii++;
                        }
                    }
                }
                V3DLONG index_pim = offsetk + offsetj + ix;
                outimg[index_pim] = arr[size - order];
            }
        }
    }
}



#endif // MED_FILTER_H
