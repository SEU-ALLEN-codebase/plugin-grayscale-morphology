/// MIT License
/// by Zuohan Zhao

#ifndef MY_IMAGE_H
#define MY_IMAGE_H


#include <v3d_interface.h>


struct MyImage
{
    QSharedPointer<std::byte> data1d;
    ImagePixelType datatype = V3D_UNKNOWN;
    V3DLONG sz[4] = {0, 0, 0, 0};
    enum BorderType { NONE, REPLICATE, REFLECT, WRAP } borderType = BorderType::NONE;

    // const accessors
    float at(V3DLONG i) const;
    float at(V3DLONG x, V3DLONG y, V3DLONG z) const;
    float operator()(V3DLONG x, V3DLONG y, V3DLONG z) const;

    // mutators
    void set(V3DLONG i, float v);
    void saturate_set(V3DLONG i, float v);
    void set(V3DLONG x, V3DLONG y, V3DLONG z, float v);
    void saturate_set(V3DLONG x, V3DLONG y, V3DLONG z, float v);

    void create(char flag = 0);
    MyImage as_type(ImagePixelType newtype, float scale = 1, bool saturate = true) const;
    void load(const QString& path, V3DPluginCallback2& cb);
    void save(const QString& path, V3DPluginCallback2& cb);
    MyImage copy() const;
    void assign(const Image4DSimple& img);
};


#endif // MY_IMAGE_H
