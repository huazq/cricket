#include <stdlib.h>

#include "cpu-libwrap.h"
#include "log.h"

static const char* DEFAULT_LIBCUDA_PATH = "/usr/local/cuda/lib64/libcudart.so";
static void *so_handle;

static const char *libwrap_cuda_path(void)
{
    const char *path = getenv("CRICKET_LIBCUDA_PATH");
    return path && path[0] ? path : DEFAULT_LIBCUDA_PATH;
}

inline void* libwrap_get_sohandle()
{
    if (!so_handle) {
        const char *cuda_path = libwrap_cuda_path();
        if ( !(so_handle = dlopen(cuda_path, RTLD_LAZY)) ) {
            LOGE(LOG_ERROR, "dlopen %s failed: %s", cuda_path, dlerror());
            so_handle = NULL;
            return 0;
        }
    }
    return so_handle;
}

inline void libwrap_pre_call(char *ret, char *name, char *parameters)
{
    LOG(LOG_DEBUG, "%s", name);
}
inline void libwrap_post_call(char *ret, char *name, char *parameters)
{
    LOG(LOG_DEBUG, "%s", name);
}
