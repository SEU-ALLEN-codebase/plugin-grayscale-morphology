#include "my_image.h"
#define AT(x,y,z) (z*sz[0]*sz[1]+y*sz[0]+x)

/// MIT License
/// by Zuohan Zhao

using namespace std;


float MyImage::at(V3DLONG x, V3DLONG y, V3DLONG z) const
{
    return at(AT(x, y, z));
}

float MyImage::at(V3DLONG i) const
{
    switch (datatype)
    {
    case V3D_UINT8:
        return ((v3d_uint8*)data1d.data())[i];
        break;
    case V3D_UINT16:
        return ((v3d_uint16*)data1d.data())[i];
        break;
    case V3D_FLOAT32:
        return ((v3d_float32*)data1d.data())[i];
        break;
    default:
        throw "MyImage: Unknown data type during getting pixel.";
    }
}


float MyImage::operator()(V3DLONG x, V3DLONG y, V3DLONG z) const
{
    switch (borderType)
    {
    case BorderType::NONE:
        if (x < 0 || x >= sz[0] || y < 0 || y >= sz[1] || z < 0 || z >= sz[2])
            return 0;
        break;
    case BorderType::REFLECT:
        while (x < 0 || x >= sz[0])
        {
            x = abs(x);
            if (x >= sz[0]) x = sz[0] * 2 - x - 1;
        }
        while (y < 0 || y >= sz[1])
        {
            y = abs(y);
            if (y >= sz[1]) y = sz[1] * 2 - y - 1;
        }
        while (z < 0 || z >= sz[2])
        {
            z = abs(z);
            if (z >= sz[2]) z = sz[2] * 2 - z - 1;
        }
        break;
    case BorderType::WRAP:
        x %= sz[0];
        if (x < 0) x += sz[0];
        y %= sz[1];
        if (y < 0) y += sz[1];
        z %= sz[2];
        if (z < 0) z += sz[2];
        break;
    case BorderType::REPLICATE:
        x = clamp(x, (V3DLONG)0, sz[0] - 1);
        y = clamp(y, (V3DLONG)0, sz[1] - 1);
        z = clamp(z, (V3DLONG)0, sz[2] - 1);
        break;
    default:
        throw "MyImage: Unknown border type during getting pixel.";
    }
    return at(x, y, z);
}


void MyImage::saturate_set(V3DLONG x, V3DLONG y, V3DLONG z, float v)
{
    saturate_set(AT(x, y, z), v);
}


void MyImage::saturate_set(V3DLONG i, float v)
{
    switch (datatype)
    {
    case V3D_UINT8:
        ((v3d_uint8*)data1d.data())[i] = clamp(round(v), 0.f, 255.f);
        break;
    case V3D_UINT16:
        ((v3d_uint16*)data1d.data())[i] = clamp(round(v), 0.f, 65535.f);
        break;
    case V3D_FLOAT32:
        ((v3d_float32*)data1d.data())[i] = clamp(v, 0.f, 1.f);
        break;
    default:
        throw "MyImage: Unknown data type during setting pixel.";
    }
}


void MyImage::set(V3DLONG x, V3DLONG y, V3DLONG z, float v)
{
    set(AT(x, y, z), v);
}


void MyImage::set(V3DLONG i, float v)
{
    switch (datatype)
    {
    case V3D_UINT8:
        ((v3d_uint8*)data1d.data())[i] = round(v);
        break;
    case V3D_UINT16:
        ((v3d_uint16*)data1d.data())[i] = round(v);
        break;
    case V3D_FLOAT32:
        ((v3d_float32*)data1d.data())[i] = v;
        break;
    default:
        throw "MyImage: Unknown data type during setting pixel.";
    }
}

void MyImage::create(char flag)
{
    auto&& nByte = sz[0] * sz[1] * sz[2] * datatype;
    data1d = QSharedPointer<byte>(new byte[nByte], default_delete<byte[]>());
    if (data1d.isNull())
        throw "MyImage: Memory allocation failed for image creation.";
    memset(data1d.data(), flag, nByte);
}


MyImage MyImage::as_type(int newtype, float scale, bool saturate) const
{
    auto out = *this;
    out.datatype = newtype;
    out.create();
    auto&& tot_sz = sz[0] * sz[1] * sz[2];
    for (V3DLONG i = 0; i < tot_sz; ++i)
    {
        auto&& v = at(i) * scale;
        if (saturate)
            out.saturate_set(i, v);
        else
            out.set(i, v);
    }
    return out;
}

void MyImage::load(const QString& path, V3DPluginCallback2& cb)
{
    v3d_uint8* p = nullptr;
    if(!simple_loadimage_wrapper(cb, path.toStdString().c_str(), p, sz, datatype))
        throw "MyImage: Loading image failed.";
    data1d = QSharedPointer<byte>((byte*)p, default_delete<byte[]>());
}

void MyImage::save(const QString& path, V3DPluginCallback2& cb)
{
    if (!simple_saveimage_wrapper(cb, path.toStdString().c_str(),
                                  (v3d_uint8*)data1d.data(), sz, datatype))
        throw "MyImage: Saving image failed.";
}


MyImage MyImage::copy() const
{
    auto out = *this;
    out.create();
    auto&& buffer_sz = sz[0] * sz[1] * sz[2];
    switch (datatype)
    {
    case V3D_UINT8:
        break;
    case V3D_UINT16:
        buffer_sz *= 2;
        break;
    case V3D_FLOAT32:
        buffer_sz *= 4;
        break;
    default:
        throw "MyImage: Unknown datatype during copying.";
    }
    memcpy(out.data1d.data(), data1d.data(), buffer_sz);
    return out;
}
