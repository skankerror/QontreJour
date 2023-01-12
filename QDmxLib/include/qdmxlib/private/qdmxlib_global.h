#ifndef QDMXLIB_GLOBAL_H
#define QDMXLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QDMXLIB_LIBRARY)
#  define QDMXLIB_EXPORT Q_DECL_EXPORT
#else
#  define QDMXLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // QDMXLIB_GLOBAL_H
