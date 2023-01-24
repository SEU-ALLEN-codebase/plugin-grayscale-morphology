/// MIT License
/// by Zuohan Zhao

#include "argparser.h"
#include <QList>
#include <QVector3D>


ArgParser::ArgParser(const V3DPluginArgList& input, V3DPluginArgList& output)
{
    if (input.size() > 0)
    {
        auto inlist  = (std::vector<char*>*)input.at(0).p;
        auto i = inlist->cbegin();
        while (i != inlist->cend())
            _inlist.append(*i++);
    }

    if (output.size() > 0)
    {
        auto outlist = (std::vector<char*>*)output.at(0).p;
        auto j = outlist->cbegin();
        while (j != outlist->cend())
            _outlist.append(*j++);
    }

    if (input.size() > 1)
    {
        auto arglist = (std::vector<char*>*)(input.at(1).p);
        for (auto i = 0; i < arglist->size(); i+=2)
            _opts[arglist->at(i)] = arglist->at(i + 1);
    }
}


QString ArgParser::input(V3DLONG i) const
{
    auto& temp = _inlist.at(i);
    qDebug() << "Parsed input file path" << i << ":" << temp;
    return temp;
}


QString ArgParser::output(V3DLONG i) const
{
    auto& temp = _outlist.at(i);
    qDebug() << "Parsed output file path" << i << ":" << temp;
    return temp;
}


bool ArgParser::parse(const QString& name, QString& out) const
{
    auto node = _opts.constFind(name);
    if (node == _opts.constEnd()) return false;

    out = node->toString();
    qDebug() << "Parsed string" << name << ":" << out;
    return true;
}


bool ArgParser::parse(const QString& name, V3DLONG& out) const
{
    auto node = _opts.constFind(name);
    if (node == _opts.constEnd()) return false;

    bool flag;
    out = node->toInt(&flag);
    if (!flag) throwf(name, node->toString(), "V3DLONG");
    qDebug() << "Parsed V3DLONG" << name << ":" << out;
    return true;
}


bool ArgParser::parse(const QString& name, float& out) const
{
    auto node = _opts.constFind(name);
    if (node == _opts.constEnd()) return false;

    bool flag;
    out = node->toFloat(&flag);
    if (!flag) throwf(name, node->toString(), "float");
    qDebug() << "Parsed float" << name << ":" << out;
    return true;
}


bool ArgParser::parse(const QString& name, QVector3D& out) const
{
    auto node = _opts.constFind(name);
    if (node == _opts.constEnd()) return false;

    auto list = node->toString().split(',');
    if (list.size() != 3)
        throw "ArgParser: Vector length must be 3.";
    for (auto i: {0, 1, 2})
    {
        bool flag;
        out[i] = list.at(i).toFloat(&flag);
        if (!flag) throwf(name, node->toString(), "float");
    }
    qDebug() << "Parsed 3D vector" << name << ":" << out;
    return true;
}


bool ArgParser::parse(const QString& name, bool& out) const
{
    auto node = _opts.constFind(name);
    if (node == _opts.constEnd()) return false;

    out = node->toBool();
    qDebug() << "Parsed bool" << name << ":" << out;
    return true;
}


void ArgParser::throwf(const QString& key, const QString& value, const QString& type) const
{
    throw QString("ArgParser: Key %1 has value %2, not matching type %3.").arg(key, value, type).toStdString().c_str();
}
