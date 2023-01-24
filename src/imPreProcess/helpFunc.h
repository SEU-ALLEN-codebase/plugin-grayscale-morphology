#ifndef IMPREPROCESS_MAIN_H
#define IMPREPROCESS_MAIN_H

#include "v3d_interface.h"
#include "fun_fft.h"
#include "morphology.h"
#include "algorithm"
#include <iostream>
using namespace std;

double cal_percentile(int* src, V3DLONG sz, double per);
void getColorMask(vector<double> &colorMask, double colorSigma, int dt);
void getGaussianMask(float* &mask, int* kernelSZ, double spaceSigmaX, double spaceSigmaY, double spaceSigmaZ);

template <class T>
void sigma_correction(T* src, const V3DLONG mysz[4], double cutoff, double gain, T*  dst, int dt)
{
    V3DLONG tolSZ = mysz[0]*mysz[1]*mysz[2]*mysz[3];

    QSharedPointer<int> src1buffer(new int[tolSZ], default_delete<int[]>());
    if (src1buffer.isNull())
        throw "Enhancement: Fail to allocate memory in sigma correction.";
    auto&& src1 = src1buffer.data();

    for (V3DLONG i=0; i<tolSZ; i++) src1[i] = src[i];

    cutoff = cal_percentile(src1, tolSZ, cutoff);

    cout<<"cutoff = "<<cutoff<<endl;

    double minfl = INFINITY, maxfl=0.0;
    for (V3DLONG i=0; i<tolSZ; i++)
    {
        if(minfl>src[i]) minfl=src[i];
        if(maxfl<src[i]) maxfl=src[i];
    }
    double scale = maxfl-minfl;
    cutoff /= scale;
    int val_max;
    switch(dt)
    {
    case V3D_UINT8:
        val_max = 255;
        break;
    case V3D_UINT16:
        val_max = 65535;
        break;
    default:
        throw "Enhancement: Invalid data type for sigma correction.";
    }

    int temp=0;
    for (V3DLONG i=0; i<tolSZ; i++)
    {
        temp = int(scale/(1+exp(gain*(cutoff-double(src[i])/scale))));
        if(temp>val_max) temp=val_max;
        dst[i] = temp;
    }
}

template <class T>
void intensity_rescale(T* src, const V3DLONG mysz[4], T* dst, int dt)
{
    V3DLONG tolSZ = mysz[0]*mysz[1]*mysz[2]*mysz[3];

    double minfl = INFINITY, maxfl=0.0;
    for (V3DLONG i=0; i<tolSZ; i++)
    {
        if(src[i]<0) src[i]=0;
        if(minfl>src[i]) minfl=src[i];
        if(maxfl<src[i]) maxfl=src[i];
    }

    cout<<"minfl = "<<minfl<<endl;
    cout<<"maxfl = "<<maxfl<<endl;

    int max_val = 0;
    switch(dt)
    {
    case V3D_UINT8: max_val = 255;
        break;
    case V3D_UINT16: max_val = 65535;
        break;
    default:
        throw "Enhancement: Invalid datatype for intensity rescale.";
    }

    for(V3DLONG i=0; i<tolSZ; i++)
    {
        dst[i] = int((src[i]-minfl)/(maxfl-minfl)*max_val);
    }
}

template <class T>
void intensity_projection(T* src, const V3DLONG mysz[4], T* dst, int flag)
{
    for(V3DLONG iy = 0; iy < mysz[1]; iy++)
    {
        V3DLONG offsetj = iy*mysz[0];
        for(V3DLONG ix = 0; ix < mysz[0]; ix++)
        {
            int max_mip = 0, min_mip=INFINITY;
            for(V3DLONG iz = 0; iz < mysz[2]; iz++)
            {
                V3DLONG offsetk = iz*mysz[1]*mysz[0];
                double datatl = src[offsetk + offsetj + ix];
                if(datatl>max_mip) max_mip = datatl;
                if(datatl<min_mip) min_mip = datatl;
            }

            if(flag==1)   ///maximum
            {
                dst[iy*mysz[0] + ix] = max_mip;
            }
            if(flag==2)   //minimum
            {
                dst[iy*mysz[0] + ix] = min_mip;
            }
        }
    }
}


template <class T>
void subtract_min(T* src, const V3DLONG mysz[4], T* dst)
{
    QSharedPointer<T> mip_buffer(new T[mysz[0]*mysz[1]], default_delete<T[]>());
    if (mip_buffer.isNull())
        throw "Enhancement: Fail to allocate memory in subtract min.";
    auto&& mip = mip_buffer.data();

    intensity_projection(src, mysz, mip, 2);

    for(V3DLONG iz = 0; iz < mysz[2]; iz++){
        V3DLONG offsetz = iz*mysz[0]*mysz[1];
        for(V3DLONG iy = 0; iy < mysz[1]; iy++){
            V3DLONG offsety = iy*mysz[0];
            for(V3DLONG ix=0; ix<mysz[0]; ix++){
                dst[offsetz + offsety + ix] = src[offsetz + offsety + ix]-mip[offsety + ix];
            }
        }
    }
}

template <class T>
void fft_filter(T* src, const V3DLONG mysz[4], T* out, int dt)
{
    V3DLONG tolSize = mysz[0]*mysz[1]*mysz[2];
    if(!(isPower(mysz[0])|isPower(mysz[1])|isPower(mysz[2])))
        throw "Enhancement: Image size is not power of 2 for fft.";

    QSharedPointer<complex_type> dst_buffer(new complex_type[tolSize], default_delete<complex_type[]>());
    if (dst_buffer.isNull())
        throw "Enhancement: Fail to allocate memory in fft.";
    auto&& dst = dst_buffer.data();

    for (V3DLONG i=0; i<tolSize; ++i)
        dst[i] = (complex_type)(1.0*src[i]);

    FFT_inplace(dst, mysz, FFT_FORWARD);

    V3DLONG sz_x  = mysz[0];
    V3DLONG sz_y  = mysz[1];
    V3DLONG sz_z = mysz[2];

    cout<<"sz_x = "<<sz_x<<endl;
    cout<<"sz_y = "<<sz_y<<endl;
    cout<<"sz_z = "<<sz_z<<endl;

    for(V3DLONG iz = 0; iz<1; iz++)
        for(V3DLONG ix = 0; ix < sz_x; ix++)  //[:,:,sz_z/2]=0
            for(V3DLONG iy = 0; iy < sz_y; iy++){
                V3DLONG offset = iz*sz_x*sz_y + iy*sz_x + ix;
                dst[offset] = (complex_type)(0);
            }

   for(V3DLONG iz = 0; iz < sz_z; iz++)  //[:,sz_y/2,:]=0
        for(V3DLONG iy = 0; iy < 1; iy++)
            for(V3DLONG ix = 0; ix < sz_x; ix++){
                V3DLONG offset = iz*sz_x*sz_y + iy*sz_x + ix;
                dst[offset] = (complex_type)(0);
            }

   for(V3DLONG iz = 0; iz < sz_z; iz++)  //[sz_x/2, :, :]=0
        for(V3DLONG iy = 0; iy < sz_y; iy++)
            for(V3DLONG ix = 0; ix < 1; ix++){
                V3DLONG offset = iz*sz_x*sz_y + iy*sz_x + ix;
                dst[offset] = (complex_type)(0);
            }

    FFT_inplace(dst, mysz, FFT_BACKWARD);

    int max_val = 0;
    switch(dt)
    {
    case V3D_UINT8: max_val = 255;
        break;
    case V3D_UINT16: max_val = 65535;
        break;
    default:
        throw "Enhancement: Invalid data type for fft.";
    }

    int temp=0;
    for (V3DLONG i=0; i<tolSize; ++i){
        temp = int(dst[i].real());
        if(temp>max_val) temp=max_val;
        if(temp<0) temp=0;
        out[i]=(T)temp;
    }
}


#endif // IMPREPROCESS_MAIN_H

