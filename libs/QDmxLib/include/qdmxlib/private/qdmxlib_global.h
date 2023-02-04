#ifndef QDMXLIB_GLOBAL_H
#define QDMXLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QDMXLIB_LIBRARY)
#  define QDMXLIB_EXPORT Q_DECL_EXPORT
#else
#  define QDMXLIB_EXPORT Q_DECL_IMPORT
#endif

class QDmxFinally
{
    using cb_t = std::function<void()>;
public:
    QDmxFinally(cb_t&& cb) :
        _cb(std::forward<cb_t>(cb))
    {}
    ~QDmxFinally() { call(); }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }

    void call() { if(_enabled && _cb) _cb(); }

protected:
    bool _enabled = true;
    cb_t _cb;
};

#endif // QDMXLIB_GLOBAL_H
