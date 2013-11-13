#ifndef TEXTCAPTURINGQPAINTER_H
#define TEXTCAPTURINGQPAINTER_H

#include <QPainter>
#include <QString>

class TextCapturingQPainter : public QPainter
{
public:
    TextCapturingQPainter();
protected:
    virtual void drawTextHook(const QString &str);
};

#endif // TEXTCAPTURINGQPAINTER_H
