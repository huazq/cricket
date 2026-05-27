#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <nvml.h>
#include <cuda_runtime_api.h>

#include "cpu_rpc_prot.h"
#include "cpu-common.h"
#include "cpu-utils.h"
#include "log.h"
#include "resource-mg.h"
#define WITH_RECORDER
#include "api-recorder.h"
#include "gsched.h"

int server_nvml_init(int restore)
{
    int ret = 0;
    if (!restore) {
        //ret &= resource_mg_init(&rm_modules, 1);
    } else {
        //ret &= resource_mg_init(&rm_modules, 0);
        //ret &= server_driver_restore("ckp");
    }
    return ret;
}

int server_nvml_deinit(void)
{
    //resource_mg_free(&rm_modules);
    return 0;
}

bool_t rpc_nvmldevicegetcount_v2_1_svc(int_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    // Workaround for pytorch expecting nvmlDeviceGetCount and cudaGetDeviceCount to be the same
    //result->err = nvmlDeviceGetCount_v2(&result->int_result_u.data);
    result->err = cudaGetDeviceCount(&result->int_result_u.data);
    LOGE(LOG_DEBUG, "%s: %d", __FUNCTION__, result->int_result_u.data);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlinitwithflags_1_svc(int flags, int *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    *result = nvmlInitWithFlags(flags);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlinit_v2_1_svc(int *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    *result = nvmlInit_v2();
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlshutdown_1_svc(int *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    *result = nvmlShutdown();
    GSCHED_RELEASE;
    return 1;
}

static bool_t nvml_string_result(nvmlReturn_t (*fn)(char *, unsigned int),
                                 str_result *result)
{
    result->str_result_u.str = malloc(128);
    if (result->str_result_u.str == NULL) {
        result->err = NVML_ERROR_MEMORY;
        return 1;
    }
    memset(result->str_result_u.str, 0, 128);
    result->err = fn(result->str_result_u.str, 128);
    return 1;
}

bool_t rpc_nvmlsystemgetdriverversion_1_svc(str_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    nvml_string_result(nvmlSystemGetDriverVersion, result);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlsystemgetnvmlversion_1_svc(str_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    nvml_string_result(nvmlSystemGetNVMLVersion, result);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlsystemgetcudadriverversion_v2_1_svc(int_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlSystemGetCudaDriverVersion_v2(&result->int_result_u.data);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlerrorstring_1_svc(int status, char **result, struct svc_req *rqstp)
{
    const char *str;
    *result = malloc(128);
    if (*result == NULL) {
        return 1;
    }
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    str = nvmlErrorString((nvmlReturn_t)status);
    strncpy(*result, str, 128);
    (*result)[127] = '\0';
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegethandlebyindex_v2_1_svc(int index, ptr_result *result, struct svc_req *rqstp)
{
    nvmlDevice_t device = NULL;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetHandleByIndex_v2((unsigned int)index, &device);
    if (result->err == NVML_SUCCESS) {
        result->ptr_result_u.ptr = (ptr)device;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetname_1_svc(ptr device, str_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->str_result_u.str = malloc(128);
    if (result->str_result_u.str == NULL) {
        result->err = NVML_ERROR_MEMORY;
        GSCHED_RELEASE;
        return 1;
    }
    memset(result->str_result_u.str, 0, 128);
    result->err = nvmlDeviceGetName((nvmlDevice_t)device, result->str_result_u.str, 128);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetuuid_1_svc(ptr device, str_result *result, struct svc_req *rqstp)
{
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->str_result_u.str = malloc(128);
    if (result->str_result_u.str == NULL) {
        result->err = NVML_ERROR_MEMORY;
        GSCHED_RELEASE;
        return 1;
    }
    memset(result->str_result_u.str, 0, 128);
    result->err = nvmlDeviceGetUUID((nvmlDevice_t)device, result->str_result_u.str, 128);
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetmemoryinfo_1_svc(ptr device, ull3_result *result, struct svc_req *rqstp)
{
    nvmlMemory_t memory;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetMemoryInfo((nvmlDevice_t)device, &memory);
    if (result->err == NVML_SUCCESS) {
        result->ull3_result_u.data.u1 = memory.total;
        result->ull3_result_u.data.u2 = memory.free;
        result->ull3_result_u.data.u3 = memory.used;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetindex_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int index;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetIndex((nvmlDevice_t)device, &index);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)index;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetpciinfo_v3_1_svc(ptr device, nvml_pci_info_result *result, struct svc_req *rqstp)
{
    nvmlPciInfo_t pci;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetPciInfo_v3((nvmlDevice_t)device, &pci);
    if (result->err == NVML_SUCCESS) {
        result->nvml_pci_info_result_u.data.busIdLegacy = strdup(pci.busIdLegacy);
        result->nvml_pci_info_result_u.data.domain = pci.domain;
        result->nvml_pci_info_result_u.data.bus = pci.bus;
        result->nvml_pci_info_result_u.data.device = pci.device;
        result->nvml_pci_info_result_u.data.pciDeviceId = pci.pciDeviceId;
        result->nvml_pci_info_result_u.data.pciSubSystemId = pci.pciSubSystemId;
        result->nvml_pci_info_result_u.data.busId = strdup(pci.busId);
        if (result->nvml_pci_info_result_u.data.busIdLegacy == NULL ||
            result->nvml_pci_info_result_u.data.busId == NULL) {
            free(result->nvml_pci_info_result_u.data.busIdLegacy);
            free(result->nvml_pci_info_result_u.data.busId);
            result->err = NVML_ERROR_MEMORY;
        }
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetpersistencemode_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    nvmlEnableState_t mode;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetPersistenceMode((nvmlDevice_t)device, &mode);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)mode;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetfanspeed_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int speed;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetFanSpeed((nvmlDevice_t)device, &speed);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)speed;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegettemperature_1_svc(ptr device, int sensor_type, int_result *result, struct svc_req *rqstp)
{
    unsigned int temp;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetTemperature((nvmlDevice_t)device, (nvmlTemperatureSensors_t)sensor_type, &temp);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)temp;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetperformancestate_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    nvmlPstates_t state;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetPerformanceState((nvmlDevice_t)device, &state);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)state;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetpowerusage_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int power;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetPowerUsage((nvmlDevice_t)device, &power);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)power;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetpowermanagementlimit_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int limit;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetPowerManagementLimit((nvmlDevice_t)device, &limit);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)limit;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegettotaleccerrors_1_svc(ptr device, int error_type, int counter_type, int_result *result, struct svc_req *rqstp)
{
    unsigned long long count;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetTotalEccErrors((nvmlDevice_t)device,
                                              (nvmlMemoryErrorType_t)error_type,
                                              (nvmlEccCounterType_t)counter_type,
                                              &count);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = count > INT_MAX ? INT_MAX : (int)count;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetmigmode_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int current;
    unsigned int pending;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetMigMode((nvmlDevice_t)device, &current, &pending);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)current;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetcomputerunningprocesses_v3_1_svc(ptr device, nvml_process_info_result *result, struct svc_req *rqstp)
{
    unsigned int count = 64;
    nvmlProcessInfo_t infos[64];
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    memset(infos, 0, sizeof(infos));
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetComputeRunningProcesses_v3((nvmlDevice_t)device, &count, infos);
    if (result->err == NVML_SUCCESS) {
        result->nvml_process_info_result_u.data.infos.infos_len = count;
        result->nvml_process_info_result_u.data.infos.infos_val = calloc(count, sizeof(nvml_process_info));
        if (count != 0 && result->nvml_process_info_result_u.data.infos.infos_val == NULL) {
            result->err = NVML_ERROR_MEMORY;
        } else {
            for (unsigned int i = 0; i < count; ++i) {
                result->nvml_process_info_result_u.data.infos.infos_val[i].pid = infos[i].pid;
                result->nvml_process_info_result_u.data.infos.infos_val[i].usedGpuMemory = infos[i].usedGpuMemory;
                result->nvml_process_info_result_u.data.infos.infos_val[i].gpuInstanceId = infos[i].gpuInstanceId;
                result->nvml_process_info_result_u.data.infos.infos_val[i].computeInstanceId = infos[i].computeInstanceId;
            }
        }
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmlsystemgetprocessname_1_svc(int pid, str_result *result, struct svc_req *rqstp)
{
    char path[64];
    FILE *f;

    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->str_result_u.str = malloc(128);
    if (result->str_result_u.str == NULL) {
        result->err = NVML_ERROR_MEMORY;
        GSCHED_RELEASE;
        return 1;
    }
    memset(result->str_result_u.str, 0, 128);
    result->err = nvmlSystemGetProcessName((unsigned int)pid, result->str_result_u.str, 128);
    if (result->err != NVML_SUCCESS || result->str_result_u.str[0] == '\0') {
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
        f = fopen(path, "r");
        if (f != NULL) {
            size_t n = fread(result->str_result_u.str, 1, 127, f);
            fclose(f);
            for (size_t i = 0; i < n; ++i) {
                if (result->str_result_u.str[i] == '\0') {
                    result->str_result_u.str[i] = ' ';
                }
            }
            result->str_result_u.str[n] = '\0';
            result->err = NVML_SUCCESS;
        }
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegeteccmode_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    nvmlEnableState_t current;
    nvmlEnableState_t pending;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetEccMode((nvmlDevice_t)device, &current, &pending);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)current;
    }
    GSCHED_RELEASE;
    return 1;
}

bool_t rpc_nvmldevicegetenforcedpowerlimit_1_svc(ptr device, int_result *result, struct svc_req *rqstp)
{
    unsigned int limit;
    LOGE(LOG_DEBUG, "%s", __FUNCTION__);
    GSCHED_RETAIN;
    result->err = nvmlDeviceGetEnforcedPowerLimit((nvmlDevice_t)device, &limit);
    if (result->err == NVML_SUCCESS) {
        result->int_result_u.data = (int)limit;
    }
    GSCHED_RELEASE;
    return 1;
}
