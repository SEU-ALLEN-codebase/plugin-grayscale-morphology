#ifndef UTILS_H
#define UTILS_H

#include <v3d_interface.h>

inline void export_marker(const QString& path, const QList<LocationSimple>& marker_list)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw "Marker file writing failed!";
    QTextStream out(&file);
    out << "#x, y, z, radius, shape, name, comment,color_r,color_g,color_b\n";
    for (auto i = marker_list.begin(); i != marker_list.end(); ++i)
        out << QString("%1, %2, %3, %4, 1, %5, %6, %7, %8, %9\n").
               arg(i->x, 0, 'f').arg(i->y, 0, 'f').arg(i->z, 0, 'f').arg(i->radius, 0, 'f').
               arg(QString::fromStdString(i->name), QString::fromStdString(i->comments)).
               arg(i->color.r).arg(i->color.g).arg(i->color.b);
    file.close();
}

#endif // UTILS_H
