#ifndef ABSTRACT_ACTIONS_H
#define ABSTRACT_ACTIONS_H

#include "handlers.h"
#include "utils.h"


class BaseAction
{
public:
    V3DPluginCallback2* callback;
    QWidget* parent;
    const V3DPluginArgList* input;
    V3DPluginArgList* output;
    virtual void init(V3DPluginCallback2 &callback, QWidget *parent)
    {
        this->callback = &callback;
        this->parent = parent;
    }
    virtual void init(const V3DPluginArgList& input, V3DPluginArgList& output, V3DPluginCallback2& callback,  QWidget* parent)
    {
        init(callback, parent);
        this->input = &input;
        this->output = &output;
    }
    virtual void exec() = 0;
    virtual void parse() {};
    virtual void dialog() {};
    int banner_len = 50;

protected:
    virtual void _flag_start(const char* title)
    {
        auto wing = qPrintable(QString((banner_len - strlen(title) - 2) / 2, '<'));
        qDebug() << "";
        qDebug() << wing << title << wing;
    }
    virtual void _flag_end()
    {
        auto wing = qPrintable(QString((banner_len - 6) / 2, '<'));
        qDebug() << "";
        qDebug() << wing << "DONE" << wing;
    }
    virtual void _step(const char* title)
    {
        auto suffix = qPrintable(QString(banner_len - strlen(title) - 3, '='));
        qDebug() << "\n>" << title << suffix;
    }
    virtual ArgParser _parse(std::initializer_list<Handler*> handlers)
    {
        qDebug() << "\n> PARSE INPUT ARGUMENTS";
        auto parser = ArgParser(*input, *output);
        for (auto h: handlers) h->parse(parser);
        return parser;
    }
};


class ImageAction: virtual public BaseAction
{
public:
    QString img_in;
    MyImage img;
    virtual void load()
    {
        _step("LOAD IMAGE");
        img.load(img_in, *callback);
    }

protected:
    ArgParser _parse(std::initializer_list<Handler*> handlers) override
    {
        auto parser = BaseAction::_parse(handlers);
        img_in = parser.input(0);
        return parser;
    }
};


class ImageFilterAction: virtual public ImageAction
{
public:
    QString img_out;
    virtual void save()
    {
        _step("SAVE IMAGE");
        img.save(img_out, *callback);
    }

protected:
    ArgParser _parse(std::initializer_list<Handler*> handlers) override
    {
        auto parser = ImageAction::_parse(handlers);
        img_out = parser.output(0);
        return parser;
    }
};


class ImageAnnotationAction: virtual public ImageAction
{
public:
    QString marker_out;
    QList<LocationSimple> marker_list;
    virtual void save()
    {
        _step("SAVE ANNOTATION");
        export_marker(marker_out, marker_list);
    }

protected:
    ArgParser _parse(std::initializer_list<Handler*> handlers) override
    {
        auto parser = ImageAction::_parse(handlers);
        marker_out = parser.output(0);
        return parser;
    }
};


class ImageFilterAnnotationAction: public ImageFilterAction, public ImageAnnotationAction
{
public:
    void load() override
    {
        ImageFilterAction::load();
    }
    void save(bool img = true, bool annotation = true)
    {
        if (img) ImageFilterAction::save();
        if (annotation) ImageAnnotationAction::save();
    }
protected:
    ArgParser _parse(std::initializer_list<Handler*> handlers) override
    {
        auto parser = ImageFilterAction::_parse(handlers);
        marker_out = parser.output(1);
        return parser;
    }
};


#endif // ABSTRACT_ACTIONS_H
