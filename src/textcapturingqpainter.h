#ifndef TEXTCAPTURINGQPAINTER_H
#define TEXTCAPTURINGQPAINTER_H

#include <QPainter>
#include <QString>
#include <QVector>

class TextCapturingQPainter : public QPainter
{
public:
    TextCapturingQPainter();
    inline QStringList& getTextlist() { return textlist; }
    inline void resetTextlist() { textlist.clear(); }
protected:
    virtual void drawTextHook(const QString &str);
private:
    QStringList textlist;
};

#endif // TEXTCAPTURINGQPAINTER_H
