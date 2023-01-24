/// MIT License
/// by Zuohan Zhao

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <v3d_interface.h>


class ArgParser
{
public:
    ArgParser(const V3DPluginArgList& input, V3DPluginArgList& output);
    QString input(V3DLONG i = 0) const;
    QString output(V3DLONG i = 0) const;

    /// all parsing functions are boolean
    /// if false, meaning not found
    /// while parsing failure would cause error
    bool parse(const QString& name, QString& out) const;
    bool parse(const QString& name, V3DLONG& out) const;
    bool parse(const QString& name, float& out) const;
    bool parse(const QString& name, QVector3D& out) const;
    bool parse(const QString& name, bool& out) const;

private:
    QVariantHash _opts;
    QStringList _inlist, _outlist;
    void throwf(const QString& key, const QString& value, const QString& type) const;

};


#endif // ARGPARSER_H
