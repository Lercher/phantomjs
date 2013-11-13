#include "textcapturingqpainter.h"
#include <iostream>


TextCapturingQPainter::TextCapturingQPainter()
{
}

void TextCapturingQPainter::drawTextHook(const QString &str)
{
    std::cout << "drawText: '" << str.toAscii().constData() << "'" << std::endl;
}
