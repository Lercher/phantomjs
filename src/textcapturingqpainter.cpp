#include "textcapturingqpainter.h"
#include <iostream>

TextCapturingQPainter::TextCapturingQPainter()
{
}

void TextCapturingQPainter::drawTextHook(const QString &str)
{
    QString deepcopy = QString::fromUtf16(str.utf16());
    // std::cout << "drawText: '" << str.toAscii().constData() << "'" << std::endl;
    textlist << deepcopy;
}
