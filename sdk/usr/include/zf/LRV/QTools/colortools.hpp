#pragma once

#include "QToolsApi.h"

class QColor;
class QString;

/// get color from a string like '255,255,0'
QTOOLS_API QColor str2col(const QString& s);

/// generate a string from color like '255,255,0'
QTOOLS_API QString col2str(const QColor& c, bool alpha = false);
