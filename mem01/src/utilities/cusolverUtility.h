#ifndef CUSOLVERUTILITY_H
#define CUSOLVERUTILITY_H

#include "esiglobal.h"
#include "glog/logging.h"
#include <cusolverDn.h>

BEGIN_NAMESPACE_ESI
#define cusolverSafeCall(_err_) cusolverSafeCall2(_err_, __FILE__, __LINE__, FUNCNAME_PORTABLE)

inline bool cusolverSafeCall2(cusolverStatus_t err, const char *file, int line, const char *func) {
    if (err != CUSOLVER_STATUS_SUCCESS) {
        char buf[1024];
        sprintf(buf, "CUSOLVER Error (in \"%s\", Line: %d, %s): %d\n", file, line, func, err);
        printf("%s", buf);
        LOG(ERROR) << buf;
        return false;
    }
    return true;
}

END_NAMESPACE_ESI

#endif // CUSOLVERUTILITY_H
