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


inline QList<LocationSimple> import_marker(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw "Marker file reading failed!";
    QTextStream in(&file);
    QList<LocationSimple> marker_list;
    while (!in.atEnd())
    {
        auto line_str = in.readLine();
        if (line_str[0]=='#' || line_str[0]=='x' || line_str[0]=='X' || line_str[0]=='\0') continue;

        QStringList qsl = line_str.trimmed().split(",");
        int qsl_count=qsl.size();
        if (qsl_count<3)   continue;

        LocationSimple S;

        S.x = qsl[0].toFloat();
        S.y = qsl[1].toFloat();
        S.z = qsl[2].toFloat();
        S.radius = (qsl_count>=4) ? qsl[3].toInt() : 0;
        S.name = (qsl_count>=6) ? qPrintable(qsl[5].trimmed()) : "";
        S.comments = (qsl_count>=7) ? qPrintable(qsl[6].trimmed()) : "";

        S.color = random_rgba8(255);
        if (qsl_count>=8) S.color.r = qsl[7].toUInt();
        if (qsl_count>=9) S.color.g = qsl[8].toUInt();
        if (qsl_count>=10) S.color.b = qsl[9].toUInt();

        S.on = true; //listLoc[i].on;        //true;

        marker_list.append(S);
    }
    file.close();
    return marker_list;
}

#endif // UTILS_H
