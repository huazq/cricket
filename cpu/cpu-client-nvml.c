#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nvml.h>
#include <stdint.h>

#include "cpu-libwrap.h"
#include "cpu_rpc_prot.h"
#include "cpu-common.h"
#include "cpu-utils.h"
#include "log.h"

#ifdef WITH_API_CNT
static int api_call_cnt = 0;
void cpu_nvml_print_api_call_cnt(void)
{
    LOG(LOG_INFO, "nvml api-call-cnt: %d", api_call_cnt);
}
#endif //WITH_API_CNT


nvmlReturn_t nvmlInitWithFlags ( unsigned int  flags )
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int result = NVML_ERROR_UNKNOWN;
    enum clnt_stat retval_1;
    retval_1 = rpc_nvmlinitwithflags_1(flags, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    return result;
}

#undef nvmlInit
nvmlReturn_t nvmlInit(void)
{
    return nvmlInitWithFlags(0);
}

nvmlReturn_t nvmlInit_v2 ( void )
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int result = NVML_ERROR_UNKNOWN;
    enum clnt_stat retval_1;
    retval_1 = rpc_nvmlinit_v2_1(&result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    return result;
}
nvmlReturn_t nvmlShutdown ( void )
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int result = NVML_ERROR_UNKNOWN;
    enum clnt_stat retval_1;
    retval_1 = rpc_nvmlshutdown_1(&result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    return result;
}

static nvmlReturn_t nvml_get_string(enum clnt_stat (*rpc_fn)(str_result *, CLIENT *),
                                    char *value, unsigned int length)
{
    str_result result;
    enum clnt_stat retval_1;
    if (value == NULL || length == 0) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    result.str_result_u.str = NULL;
    retval_1 = rpc_fn(&result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        strncpy(value, result.str_result_u.str, length);
        value[length - 1] = '\0';
    }
    if (result.str_result_u.str != NULL) {
        free(result.str_result_u.str);
    }
    return result.err;
}

nvmlReturn_t nvmlSystemGetDriverVersion(char *version, unsigned int length)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    return nvml_get_string(rpc_nvmlsystemgetdriverversion_1, version, length);
}

nvmlReturn_t nvmlSystemGetNVMLVersion(char *version, unsigned int length)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    return nvml_get_string(rpc_nvmlsystemgetnvmlversion_1, version, length);
}

nvmlReturn_t nvmlSystemGetCudaDriverVersion_v2(int *cudaDriverVersion)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int_result result;
    enum clnt_stat retval_1;
    if (cudaDriverVersion == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmlsystemgetcudadriverversion_v2_1(&result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *cudaDriverVersion = result.int_result_u.data;
    }
    return result.err;
}

const char *nvmlErrorString(nvmlReturn_t result)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    static char str[128];
    char *rpc_result = NULL;
    enum clnt_stat retval_1;
    retval_1 = rpc_nvmlerrorstring_1((int)result, &rpc_result, clnt);
    if (retval_1 != RPC_SUCCESS || rpc_result == NULL) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return "Unknown Error";
    }
    strncpy(str, rpc_result, sizeof(str));
    str[sizeof(str) - 1] = '\0';
    free(rpc_result);
    return str;
}
static nvmlReturn_t nvml_get_int_device(enum clnt_stat (*rpc_fn)(ptr, int_result *, CLIENT *),
                                         nvmlDevice_t device, int *value)
{
    int_result result;
    enum clnt_stat retval_1;

    if (value == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_fn((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *value = result.int_result_u.data;
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetHandleByIndex_v2(unsigned int index, nvmlDevice_t *device)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    ptr_result result;
    enum clnt_stat retval_1;
    if (device == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegethandlebyindex_v2_1((int)index, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *device = (nvmlDevice_t)result.ptr_result_u.ptr;
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetName(nvmlDevice_t device, char *name, unsigned int length)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    str_result result;
    enum clnt_stat retval_1;
    if (name == NULL || length == 0) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    result.str_result_u.str = NULL;
    retval_1 = rpc_nvmldevicegetname_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        strncpy(name, result.str_result_u.str, length);
        name[length - 1] = '\0';
    }
    if (result.str_result_u.str != NULL) {
        free(result.str_result_u.str);
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetUUID(nvmlDevice_t device, char *uuid, unsigned int length)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    str_result result;
    enum clnt_stat retval_1;
    if (uuid == NULL || length == 0) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    result.str_result_u.str = NULL;
    retval_1 = rpc_nvmldevicegetuuid_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        strncpy(uuid, result.str_result_u.str, length);
        uuid[length - 1] = '\0';
    }
    if (result.str_result_u.str != NULL) {
        free(result.str_result_u.str);
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetMemoryInfo(nvmlDevice_t device, nvmlMemory_t *memory)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    ull3_result result;
    enum clnt_stat retval_1;
    if (memory == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetmemoryinfo_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        memory->total = result.ull3_result_u.data.u1;
        memory->free = result.ull3_result_u.data.u2;
        memory->used = result.ull3_result_u.data.u3;
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetMemoryInfo_v2(nvmlDevice_t device, nvmlMemory_v2_t *memory)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    nvmlMemory_t memory_v1;
    nvmlReturn_t ret;
    if (memory == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvmlDeviceGetMemoryInfo(device, &memory_v1);
    if (ret == NVML_SUCCESS) {
        memory->version = nvmlMemory_v2;
        memory->total = memory_v1.total;
        memory->reserved = 0;
        memory->free = memory_v1.free;
        memory->used = memory_v1.used;
    }
    return ret;
}

nvmlReturn_t nvmlEventSetCreate(nvmlEventSet_t *set)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    static int event_set;
    if (set == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *set = (nvmlEventSet_t)&event_set;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceRegisterEvents(nvmlDevice_t device, unsigned long long eventTypes, nvmlEventSet_t set)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    (void)device;
    (void)eventTypes;
    if (set == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetSupportedEventTypes(nvmlDevice_t device, unsigned long long *eventTypes)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    (void)device;
    if (eventTypes == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *eventTypes = nvmlEventTypeNone;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlEventSetWait_v2(nvmlEventSet_t set, nvmlEventData_t *data, unsigned int timeoutms)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    (void)data;
    (void)timeoutms;
    if (set == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    return NVML_ERROR_TIMEOUT;
}

nvmlReturn_t nvmlEventSetFree(nvmlEventSet_t set)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    if (set == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetIndex(nvmlDevice_t device, unsigned int *index)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int_result result;
    enum clnt_stat retval_1;
    if (index == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetindex_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *index = (unsigned int)result.int_result_u.data;
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetMaxMigDeviceCount(nvmlDevice_t device, unsigned int *count)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    (void)device;
    if (count == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *count = 0;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetPciInfo_v3(nvmlDevice_t device, nvmlPciInfo_t *pci)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    nvml_pci_info_result result;
    enum clnt_stat retval_1;
    memset(&result, 0, sizeof(result));
    if (pci == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetpciinfo_v3_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        memset(pci, 0, sizeof(*pci));
        strncpy(pci->busIdLegacy, result.nvml_pci_info_result_u.data.busIdLegacy,
                sizeof(pci->busIdLegacy));
        pci->busIdLegacy[sizeof(pci->busIdLegacy) - 1] = '\0';
        pci->domain = result.nvml_pci_info_result_u.data.domain;
        pci->bus = result.nvml_pci_info_result_u.data.bus;
        pci->device = result.nvml_pci_info_result_u.data.device;
        pci->pciDeviceId = result.nvml_pci_info_result_u.data.pciDeviceId;
        pci->pciSubSystemId = result.nvml_pci_info_result_u.data.pciSubSystemId;
        strncpy(pci->busId, result.nvml_pci_info_result_u.data.busId, sizeof(pci->busId));
        pci->busId[sizeof(pci->busId) - 1] = '\0';
    }
    return result.err;
}

nvmlReturn_t nvmlDeviceGetPciInfoExt(nvmlDevice_t device, nvmlPciInfoExt_t *pci)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    nvmlPciInfo_t pci_v3;
    nvmlReturn_t ret;
    if (pci == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvmlDeviceGetPciInfo_v3(device, &pci_v3);
    if (ret == NVML_SUCCESS) {
        memset(pci, 0, sizeof(*pci));
        pci->version = nvmlPciInfoExt_v1;
        pci->domain = pci_v3.domain;
        pci->bus = pci_v3.bus;
        pci->device = pci_v3.device;
        pci->pciDeviceId = pci_v3.pciDeviceId;
        pci->pciSubSystemId = pci_v3.pciSubSystemId;
        strncpy(pci->busId, pci_v3.busId, sizeof(pci->busId));
        pci->busId[sizeof(pci->busId) - 1] = '\0';
    }
    return ret;
}

nvmlReturn_t nvmlDeviceGetPersistenceMode(nvmlDevice_t device, nvmlEnableState_t *mode)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int_result result;
    enum clnt_stat retval_1;
    if (mode == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetpersistencemode_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *mode = (nvmlEnableState_t)result.int_result_u.data;
    }
    return result.err;
}

static nvmlReturn_t nvmlExportUnsupported(void)
{
    return NVML_ERROR_NOT_SUPPORTED;
}

static nvmlReturn_t nvmlExportDeviceQuery(void *context, ...)
{
    (void)context;
    return NVML_SUCCESS;
}

static nvmlReturn_t nvmlExportDeviceGetBusId(unsigned int index, char *busId, unsigned int length)
{
    nvmlDevice_t device;
    nvmlPciInfo_t pci;
    nvmlReturn_t ret;

    if (busId == NULL || length == 0) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvmlDeviceGetHandleByIndex_v2(index, &device);
    if (ret != NVML_SUCCESS) {
        return ret;
    }
    ret = nvmlDeviceGetPciInfo_v3(device, &pci);
    if (ret != NVML_SUCCESS) {
        return ret;
    }
    if (snprintf(busId, length, "%04x:%02x:%02x.0", pci.domain, pci.bus, pci.device) >= (int)length) {
        return NVML_ERROR_INSUFFICIENT_SIZE;
    }
    return NVML_SUCCESS;
}

static const void *nvml_export_table[128] = {
    [1 ... 127] = nvmlExportUnsupported,
    [0] = (const void *)0x870,
    [26] = nvmlExportDeviceQuery,
    [81] = nvmlExportDeviceGetBusId,
};
nvmlReturn_t nvmlInternalGetExportTable(const void **ppExportTable, const void *pExportTableId)
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    if (ppExportTable == NULL || pExportTableId == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    const unsigned char *id = (const unsigned char *)pExportTableId;
    if (id[0] == 0xc4 && id[1] == 0xfe && id[2] == 0x3e && id[3] == 0x6c &&
        id[4] == 0xc9 && id[5] == 0x8f && id[6] == 0x6c && id[7] == 0x4e &&
        id[8] == 0xa3 && id[9] == 0x27 && id[10] == 0xee && id[11] == 0x69 &&
        id[12] == 0x6e && id[13] == 0x12 && id[14] == 0xf7 && id[15] == 0xc4) {
        *ppExportTable = nvml_export_table;
        return NVML_SUCCESS;
    }
    *ppExportTable = NULL;
    return NVML_ERROR_FUNCTION_NOT_FOUND;
}

DEF_FN(nvmlReturn_t, nvmlDeviceGetAPIRestriction, nvmlDevice_t, device, nvmlRestrictedAPI_t, apiType, nvmlEnableState_t*, isRestricted )
DEF_FN(nvmlReturn_t, nvmlDeviceGetAdaptiveClockInfoStatus, nvmlDevice_t, device, unsigned int*, adaptiveClockStatus )
DEF_FN(nvmlReturn_t, nvmlDeviceGetApplicationsClock, nvmlDevice_t, device, nvmlClockType_t, clockType, unsigned int*, clockMHz )
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceGetArchitecture, nvmlDevice_t, device, nvmlDeviceArchitecture_t*, arch )
DEF_FN(nvmlReturn_t, nvmlDeviceGetAttributes_v2, nvmlDevice_t, device, nvmlDeviceAttributes_t*, attributes )
#endif
DEF_FN(nvmlReturn_t, nvmlDeviceGetAutoBoostedClocksEnabled, nvmlDevice_t, device, nvmlEnableState_t*, isEnabled, nvmlEnableState_t*, defaultIsEnabled )
nvmlReturn_t nvmlDeviceGetBAR1MemoryInfo(nvmlDevice_t device, nvmlBAR1Memory_t *bar1Memory)
{
    (void)device;
    if (bar1Memory == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    memset(bar1Memory, 0, sizeof(*bar1Memory));
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetBoardId, nvmlDevice_t, device, unsigned int*, boardId )
DEF_FN(nvmlReturn_t, nvmlDeviceGetBoardPartNumber, nvmlDevice_t, device, char*, partNumber, unsigned int,  length )
DEF_FN(nvmlReturn_t, nvmlDeviceGetBrand, nvmlDevice_t, device, nvmlBrandType_t*, type )
DEF_FN(nvmlReturn_t, nvmlDeviceGetBridgeChipInfo, nvmlDevice_t, device, nvmlBridgeChipHierarchy_t*, bridgeHierarchy )
DEF_FN(nvmlReturn_t, nvmlDeviceGetClock, nvmlDevice_t, device, nvmlClockType_t, clockType, nvmlClockId_t, clockId, unsigned int*, clockMHz )
DEF_FN(nvmlReturn_t, nvmlDeviceGetClockInfo, nvmlDevice_t, device, nvmlClockType_t, type, unsigned int*, clock )
nvmlReturn_t nvmlDeviceGetComputeMode(nvmlDevice_t device, nvmlComputeMode_t *mode)
{
    (void)device;
    if (mode == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *mode = NVML_COMPUTEMODE_DEFAULT;
    return NVML_SUCCESS;
}
nvmlReturn_t nvmlDeviceGetComputeRunningProcesses_v3(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos)
{
    nvml_process_info_result result;
    enum clnt_stat retval_1;
    unsigned int count;

    memset(&result, 0, sizeof(result));
    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetcomputerunningprocesses_v3_1((ptr)device, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err != NVML_SUCCESS) {
        return result.err;
    }
    count = result.nvml_process_info_result_u.data.infos.infos_len;
    if (infos == NULL || *infoCount < count) {
        *infoCount = count;
        return NVML_ERROR_INSUFFICIENT_SIZE;
    }
    for (unsigned int i = 0; i < count; ++i) {
        infos[i].pid = result.nvml_process_info_result_u.data.infos.infos_val[i].pid;
        infos[i].usedGpuMemory = result.nvml_process_info_result_u.data.infos.infos_val[i].usedGpuMemory;
        infos[i].gpuInstanceId = result.nvml_process_info_result_u.data.infos.infos_val[i].gpuInstanceId;
        infos[i].computeInstanceId = result.nvml_process_info_result_u.data.infos.infos_val[i].computeInstanceId;
    }
    *infoCount = count;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetComputeRunningProcesses_v2(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_v2_t *infos)
{
    return nvmlDeviceGetComputeRunningProcesses_v3(device, infoCount, (nvmlProcessInfo_t *)infos);
}

#undef nvmlDeviceGetComputeRunningProcesses
nvmlReturn_t nvmlDeviceGetComputeRunningProcesses(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_v1_t *infos)
{
    nvmlProcessInfo_t infos_v2[64];
    unsigned int count;
    nvmlReturn_t ret;

    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    count = *infoCount;
    if (count > 64) {
        count = 64;
    }
    ret = nvmlDeviceGetComputeRunningProcesses_v3(device, &count, infos == NULL ? NULL : infos_v2);
    if (ret == NVML_SUCCESS && infos != NULL) {
        for (unsigned int i = 0; i < count; ++i) {
            infos[i].pid = infos_v2[i].pid;
            infos[i].usedGpuMemory = infos_v2[i].usedGpuMemory;
        }
    }
    *infoCount = count;
    return ret;
}
nvmlReturn_t nvmlDeviceGetCount_v2(unsigned int* deviceCount )
{
#ifdef WITH_API_CNT
    api_call_cnt++;
#endif //WITH_API_CNT
    int_result result;
    enum clnt_stat retval_1;
    if (deviceCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegetcount_v2_1(&result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
    }
    if (result.err == 0) {
        *deviceCount = result.int_result_u.data;
    }
    return result.err;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetCudaComputeCapability, nvmlDevice_t, device, int*, major, int*, minor )
DEF_FN(nvmlReturn_t, nvmlDeviceGetCurrPcieLinkGeneration, nvmlDevice_t, device, unsigned int*, currLinkGen )
DEF_FN(nvmlReturn_t, nvmlDeviceGetCurrPcieLinkWidth, nvmlDevice_t, device, unsigned int*, currLinkWidth )
nvmlReturn_t nvmlDeviceGetCurrentClocksThrottleReasons(nvmlDevice_t device, unsigned long long *clocksThrottleReasons)
{
    (void)device;
    if (clocksThrottleReasons == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *clocksThrottleReasons = 0;
    return NVML_SUCCESS;
}
nvmlReturn_t nvmlDeviceGetDecoderUtilization(nvmlDevice_t device, unsigned int *utilization, unsigned int *samplingPeriodUs)
{
    (void)device;
    if (utilization == NULL || samplingPeriodUs == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *utilization = 0;
    *samplingPeriodUs = 0;
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetDefaultApplicationsClock, nvmlDevice_t, device, nvmlClockType_t, clockType, unsigned int*, clockMHz )
DEF_FN(nvmlReturn_t, nvmlDeviceGetDefaultEccMode, nvmlDevice_t, device, nvmlEnableState_t*, defaultMode )
DEF_FN(nvmlReturn_t, nvmlDeviceGetDetailedEccErrors, nvmlDevice_t, device, nvmlMemoryErrorType_t, errorType, nvmlEccCounterType_t, counterType, nvmlEccErrorCounts_t*, eccCounts )
nvmlReturn_t nvmlDeviceGetDisplayActive(nvmlDevice_t device, nvmlEnableState_t *isActive)
{
    (void)device;
    if (isActive == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *isActive = NVML_FEATURE_DISABLED;
    return NVML_SUCCESS;
}
nvmlReturn_t nvmlDeviceGetDisplayMode(nvmlDevice_t device, nvmlEnableState_t *display)
{
    (void)device;
    if (display == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *display = NVML_FEATURE_DISABLED;
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetDriverModel, nvmlDevice_t, device, nvmlDriverModel_t*, current, nvmlDriverModel_t*, pending )
nvmlReturn_t nvmlDeviceGetEccMode(nvmlDevice_t device, nvmlEnableState_t *current, nvmlEnableState_t *pending)
{
    int value;
    nvmlReturn_t ret;

    if (current == NULL || pending == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegeteccmode_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *current = (nvmlEnableState_t)value;
        *pending = (nvmlEnableState_t)value;
    }
    return ret;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetEncoderCapacity, nvmlDevice_t, device, nvmlEncoderType_t, encoderQueryType, unsigned int*, encoderCapacity )
DEF_FN(nvmlReturn_t, nvmlDeviceGetEncoderSessions, nvmlDevice_t, device, unsigned int*, sessionCount, nvmlEncoderSessionInfo_t*, sessionInfos )
DEF_FN(nvmlReturn_t, nvmlDeviceGetEncoderStats, nvmlDevice_t, device, unsigned int*, sessionCount, unsigned int*, averageFps, unsigned int*, averageLatency )
nvmlReturn_t nvmlDeviceGetEncoderUtilization(nvmlDevice_t device, unsigned int *utilization, unsigned int *samplingPeriodUs)
{
    (void)device;
    if (utilization == NULL || samplingPeriodUs == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *utilization = 0;
    *samplingPeriodUs = 0;
    return NVML_SUCCESS;
}
nvmlReturn_t nvmlDeviceGetEnforcedPowerLimit(nvmlDevice_t device, unsigned int *limit)
{
    int value;
    nvmlReturn_t ret;

    if (limit == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetenforcedpowerlimit_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *limit = (unsigned int)value;
    }
    return ret;
}


DEF_FN(nvmlReturn_t, nvmlDeviceGetFBCSessions, nvmlDevice_t, device, unsigned int*, sessionCount, nvmlFBCSessionInfo_t*, sessionInfo )
DEF_FN(nvmlReturn_t, nvmlDeviceGetFBCStats, nvmlDevice_t, device, nvmlFBCStats_t*, fbcStats )
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceGetFanControlPolicy_v2, nvmlDevice_t, device, unsigned int,  fan, nvmlFanControlPolicy_t*, policy )
#endif
nvmlReturn_t nvmlDeviceGetFanSpeed(nvmlDevice_t device, unsigned int *speed)
{
    int value;
    nvmlReturn_t ret;

    if (speed == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetfanspeed_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *speed = (unsigned int)value;
    }
    return ret;
}
nvmlReturn_t nvmlDeviceGetFanSpeed_v2(nvmlDevice_t device, unsigned int fan, unsigned int *speed)
{
    (void)fan;
    return nvmlDeviceGetFanSpeed(device, speed);
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetGpuMaxPcieLinkGeneration, nvmlDevice_t, device, unsigned int*, maxLinkGenDevice )
DEF_FN(nvmlReturn_t, nvmlDeviceGetGpuOperationMode, nvmlDevice_t, device, nvmlGpuOperationMode_t*, current, nvmlGpuOperationMode_t*, pending )
nvmlReturn_t nvmlDeviceGetGraphicsRunningProcesses_v3(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos)
{
    (void)device;
    (void)infos;
    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *infoCount = 0;
    return NVML_SUCCESS;
}

#undef nvmlDeviceGetGraphicsRunningProcesses
nvmlReturn_t nvmlDeviceGetGraphicsRunningProcesses(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_v1_t *infos)
{
    (void)device;
    (void)infos;
    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *infoCount = 0;
    return NVML_SUCCESS;
}

#undef nvmlDeviceGetMPSComputeRunningProcesses
nvmlReturn_t nvmlDeviceGetMPSComputeRunningProcesses(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_v1_t *infos)
{
    (void)device;
    (void)infos;
    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *infoCount = 0;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetRunningProcessDetailList(nvmlDevice_t device, nvmlProcessDetailList_t *plist)
{
    nvmlProcessInfo_t infos[64];
    unsigned int count = 64;
    nvmlReturn_t ret;

    if (plist == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvmlDeviceGetComputeRunningProcesses_v3(device, &count, infos);
    if (ret != NVML_SUCCESS) {
        return ret;
    }
    if (plist->procArray == NULL || plist->numProcArrayEntries < count) {
        plist->numProcArrayEntries = count;
        return NVML_ERROR_INSUFFICIENT_SIZE;
    }
    for (unsigned int i = 0; i < count; ++i) {
        plist->procArray[i].pid = infos[i].pid;
        plist->procArray[i].usedGpuMemory = infos[i].usedGpuMemory;
        plist->procArray[i].gpuInstanceId = infos[i].gpuInstanceId;
        plist->procArray[i].computeInstanceId = infos[i].computeInstanceId;
        plist->procArray[i].usedGpuCcProtectedMemory = 0;
    }
    plist->numProcArrayEntries = count;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlSystemGetProcessName(unsigned int pid, char *name, unsigned int length)
{
    str_result result;
    enum clnt_stat retval_1;

    if (name == NULL || length == 0) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    memset(&result, 0, sizeof(result));
    retval_1 = rpc_nvmlsystemgetprocessname_1((int)pid, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        strncpy(name, result.str_result_u.str, length);
        name[length - 1] = '\0';
    } else {
        snprintf(name, length, "pid-%u", pid);
        result.err = NVML_SUCCESS;
    }
    if (result.str_result_u.str != NULL) {
        free(result.str_result_u.str);
    }
    return result.err;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetHandleByPciBusId_v2, const char*, pciBusId, nvmlDevice_t*, device )
DEF_FN(nvmlReturn_t, nvmlDeviceGetHandleBySerial, const char*, serial, nvmlDevice_t*, device )
DEF_FN(nvmlReturn_t, nvmlDeviceGetHandleByUUID, const char*, uuid, nvmlDevice_t*, device )
DEF_FN(nvmlReturn_t, nvmlDeviceGetInforomConfigurationChecksum, nvmlDevice_t, device, unsigned int*, checksum )
DEF_FN(nvmlReturn_t, nvmlDeviceGetInforomImageVersion, nvmlDevice_t, device, char*, version, unsigned int,  length )
DEF_FN(nvmlReturn_t, nvmlDeviceGetInforomVersion, nvmlDevice_t, device, nvmlInforomObject_t, object, char*, version, unsigned int,  length )
DEF_FN(nvmlReturn_t, nvmlDeviceGetIrqNum, nvmlDevice_t, device, unsigned int*, irqNum )
nvmlReturn_t nvmlDeviceGetMPSComputeRunningProcesses_v3(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos)
{
    (void)device;
    (void)infos;
    if (infoCount == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *infoCount = 0;
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetMaxClockInfo, nvmlDevice_t, device, nvmlClockType_t, type, unsigned int*, clock )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMaxCustomerBoostClock, nvmlDevice_t, device, nvmlClockType_t, clockType, unsigned int*, clockMHz )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMaxPcieLinkGeneration, nvmlDevice_t, device, unsigned int*, maxLinkGen )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMaxPcieLinkWidth, nvmlDevice_t, device, unsigned int*, maxLinkWidth )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMemoryBusWidth, nvmlDevice_t, device, unsigned int*, busWidth )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMemoryErrorCounter, nvmlDevice_t, device, nvmlMemoryErrorType_t, errorType, nvmlEccCounterType_t, counterType, nvmlMemoryLocation_t, locationType, unsigned long long*, count )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMinMaxFanSpeed, nvmlDevice_t, device, unsigned int*, minSpeed, unsigned int*, maxSpeed )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMinorNumber, nvmlDevice_t, device, unsigned int*, minorNumber )
DEF_FN(nvmlReturn_t, nvmlDeviceGetMultiGpuBoard, nvmlDevice_t, device, unsigned int*, multiGpuBool )
DEF_FN(nvmlReturn_t, nvmlDeviceGetNumFans, nvmlDevice_t, device, unsigned int*, numFans )
DEF_FN(nvmlReturn_t, nvmlDeviceGetNumGpuCores, nvmlDevice_t, device, unsigned int*, numCores )
DEF_FN(nvmlReturn_t, nvmlDeviceGetP2PStatus, nvmlDevice_t, device1, nvmlDevice_t, device2, nvmlGpuP2PCapsIndex_t, p2pIndex, nvmlGpuP2PStatus_t*, p2pStatus )
DEF_FN(nvmlReturn_t, nvmlDeviceGetPcieLinkMaxSpeed, nvmlDevice_t, device, unsigned int*, maxSpeed )
DEF_FN(nvmlReturn_t, nvmlDeviceGetPcieReplayCounter, nvmlDevice_t, device, unsigned int*, value )
DEF_FN(nvmlReturn_t, nvmlDeviceGetPcieSpeed, nvmlDevice_t, device, unsigned int*, pcieSpeed )
DEF_FN(nvmlReturn_t, nvmlDeviceGetPcieThroughput, nvmlDevice_t, device, nvmlPcieUtilCounter_t, counter, unsigned int*, value )
nvmlReturn_t nvmlDeviceGetPerformanceState(nvmlDevice_t device, nvmlPstates_t *pState)
{
    int value;
    nvmlReturn_t ret;

    if (pState == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetperformancestate_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *pState = (nvmlPstates_t)value;
    }
    return ret;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetPowerManagementDefaultLimit, nvmlDevice_t, device, unsigned int*, defaultLimit )
nvmlReturn_t nvmlDeviceGetPowerManagementLimit(nvmlDevice_t device, unsigned int *limit)
{
    int value;
    nvmlReturn_t ret;

    if (limit == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetpowermanagementlimit_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *limit = (unsigned int)value;
    }
    return ret;
}
nvmlReturn_t nvmlDeviceGetPowerManagementLimitConstraints(nvmlDevice_t device, unsigned int *minLimit, unsigned int *maxLimit)
{
    (void)device;
    if (minLimit == NULL || maxLimit == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *minLimit = 0;
    *maxLimit = 0;
    return NVML_ERROR_NOT_SUPPORTED;
}
nvmlReturn_t nvmlDeviceGetPowerManagementMode(nvmlDevice_t device, nvmlEnableState_t *mode)
{
    (void)device;
    if (mode == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *mode = NVML_FEATURE_DISABLED;
    return NVML_ERROR_NOT_SUPPORTED;
}
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceGetPowerSource, nvmlDevice_t, device, nvmlPowerSource_t*, powerSource )
#endif
DEF_FN(nvmlReturn_t, nvmlDeviceGetPowerState, nvmlDevice_t, device, nvmlPstates_t*, pState )
nvmlReturn_t nvmlDeviceGetPowerUsage(nvmlDevice_t device, unsigned int *power)
{
    int value;
    nvmlReturn_t ret;

    if (power == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetpowerusage_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *power = (unsigned int)value;
    }
    return ret;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetRemappedRows, nvmlDevice_t, device, unsigned int*, corrRows, unsigned int*, uncRows, unsigned int*, isPending, unsigned int*, failureOccurred )
DEF_FN(nvmlReturn_t, nvmlDeviceGetRetiredPages, nvmlDevice_t, device, nvmlPageRetirementCause_t, cause, unsigned int*, pageCount, unsigned long long*, addresses )
DEF_FN(nvmlReturn_t, nvmlDeviceGetRetiredPagesPendingStatus, nvmlDevice_t, device, nvmlEnableState_t*, isPending )
DEF_FN(nvmlReturn_t, nvmlDeviceGetRetiredPages_v2, nvmlDevice_t, device, nvmlPageRetirementCause_t, cause, unsigned int*, pageCount, unsigned long long*, addresses, unsigned long long*, timestamps )
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceGetRowRemapperHistogram, nvmlDevice_t, device, nvmlRowRemapperHistogramValues_t*, values )
#endif
DEF_FN(nvmlReturn_t, nvmlDeviceGetSamples, nvmlDevice_t, device, nvmlSamplingType_t, type, unsigned long long, lastSeenTimeStamp, nvmlValueType_t*, sampleValType, unsigned int*, sampleCount, nvmlSample_t*, samples )
DEF_FN(nvmlReturn_t, nvmlDeviceGetSerial, nvmlDevice_t, device, char*, serial, unsigned int,  length )
nvmlReturn_t nvmlDeviceGetSupportedClocksThrottleReasons(nvmlDevice_t device, unsigned long long *supportedClocksThrottleReasons)
{
    (void)device;
    if (supportedClocksThrottleReasons == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *supportedClocksThrottleReasons = 0;
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetSupportedGraphicsClocks, nvmlDevice_t, device, unsigned int,  memoryClockMHz, unsigned int*, count, unsigned int*, clocksMHz )
DEF_FN(nvmlReturn_t, nvmlDeviceGetSupportedMemoryClocks, nvmlDevice_t, device, unsigned int*, count, unsigned int*, clocksMHz )
DEF_FN(nvmlReturn_t, nvmlDeviceGetTargetFanSpeed, nvmlDevice_t, device, unsigned int,  fan, unsigned int*, targetSpeed )
nvmlReturn_t nvmlDeviceGetTemperature(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int *temp)
{
    int_result result;
    enum clnt_stat retval_1;

    if (temp == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegettemperature_1((ptr)device, (int)sensorType, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *temp = (unsigned int)result.int_result_u.data;
    }
    return result.err;
}

#ifndef nvmlTemperature_v1
typedef struct {
    unsigned int version;
    nvmlTemperatureSensors_t sensorType;
    int temperature;
} nvmlTemperature_t;
#endif

nvmlReturn_t nvmlDeviceGetTemperatureV(nvmlDevice_t device, nvmlTemperature_t *temperature)
{
    unsigned int temp;
    nvmlReturn_t ret;

    if (temperature == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvmlDeviceGetTemperature(device, temperature->sensorType, &temp);
    if (ret == NVML_SUCCESS) {
        temperature->temperature = (int)temp;
    }
    return ret;
}
nvmlReturn_t nvmlDeviceGetTemperatureThreshold(nvmlDevice_t device, nvmlTemperatureThresholds_t thresholdType, unsigned int *temp)
{
    (void)device;
    (void)thresholdType;
    if (temp == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    *temp = 0;
    return NVML_ERROR_NOT_SUPPORTED;
}
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceGetThermalSettings, nvmlDevice_t, device, unsigned int,  sensorIndex, nvmlGpuThermalSettings_t*, pThermalSettings )
#endif
DEF_FN(nvmlReturn_t, nvmlDeviceGetTopologyCommonAncestor, nvmlDevice_t, device1, nvmlDevice_t, device2, nvmlGpuTopologyLevel_t*, pathInfo )
DEF_FN(nvmlReturn_t, nvmlDeviceGetTopologyNearestGpus, nvmlDevice_t, device, nvmlGpuTopologyLevel_t, level, unsigned int*, count, nvmlDevice_t*, deviceArray )
nvmlReturn_t nvmlDeviceGetTotalEccErrors(nvmlDevice_t device, nvmlMemoryErrorType_t errorType,
                                         nvmlEccCounterType_t counterType, unsigned long long *eccCounts)
{
    int_result result;
    enum clnt_stat retval_1;

    if (eccCounts == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    retval_1 = rpc_nvmldevicegettotaleccerrors_1((ptr)device, (int)errorType, (int)counterType, &result, clnt);
    if (retval_1 != RPC_SUCCESS) {
        LOGE(LOG_ERROR, "call failed: %s", __FUNCTION__);
        return NVML_ERROR_UNKNOWN;
    }
    if (result.err == NVML_SUCCESS) {
        *eccCounts = (unsigned long long)result.int_result_u.data;
    }
    return result.err;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetTotalEnergyConsumption, nvmlDevice_t, device, unsigned long long*, energy )
nvmlReturn_t nvmlDeviceGetUtilizationRates(nvmlDevice_t device, nvmlUtilization_t *utilization)
{
    (void)device;
    if (utilization == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    utilization->gpu = 0;
    utilization->memory = 0;
    return NVML_SUCCESS;
}
DEF_FN(nvmlReturn_t, nvmlDeviceGetVbiosVersion, nvmlDevice_t, device, char*, version, unsigned int,  length )
DEF_FN(nvmlReturn_t, nvmlDeviceGetViolationStatus, nvmlDevice_t, device, nvmlPerfPolicyType_t, perfPolicyType, nvmlViolationTime_t*, violTime )
DEF_FN(nvmlReturn_t, nvmlDeviceOnSameBoard, nvmlDevice_t, device1, nvmlDevice_t, device2, int*, onSameBoard )
DEF_FN(nvmlReturn_t, nvmlDeviceResetApplicationsClocks, nvmlDevice_t, device )
DEF_FN(nvmlReturn_t, nvmlDeviceSetAutoBoostedClocksEnabled, nvmlDevice_t, device, nvmlEnableState_t, enabled )
DEF_FN(nvmlReturn_t, nvmlDeviceSetDefaultAutoBoostedClocksEnabled, nvmlDevice_t, device, nvmlEnableState_t, enabled, unsigned int,  flags )
DEF_FN(nvmlReturn_t, nvmlDeviceSetDefaultFanSpeed_v2, nvmlDevice_t, device, unsigned int,  fan )
#if NVML_API_VERSION >= 12
DEF_FN(nvmlReturn_t, nvmlDeviceSetFanControlPolicy, nvmlDevice_t, device, unsigned int,  fan, nvmlFanControlPolicy_t, policy )
#endif
DEF_FN(nvmlReturn_t, nvmlDeviceSetTemperatureThreshold, nvmlDevice_t, device, nvmlTemperatureThresholds_t, thresholdType, int*, temp )
nvmlReturn_t nvmlDeviceValidateInforom(nvmlDevice_t device)
{
    (void)device;
    return NVML_SUCCESS;
}

nvmlReturn_t nvmlDeviceGetMigMode(nvmlDevice_t device, unsigned int *currentMode, unsigned int *pendingMode)
{
    int value;
    nvmlReturn_t ret;

    if (currentMode == NULL || pendingMode == NULL) {
        return NVML_ERROR_INVALID_ARGUMENT;
    }
    ret = nvml_get_int_device(rpc_nvmldevicegetmigmode_1, device, &value);
    if (ret == NVML_SUCCESS) {
        *currentMode = (unsigned int)value;
        *pendingMode = (unsigned int)value;
    } else if (ret == NVML_ERROR_NOT_SUPPORTED) {
        *currentMode = NVML_DEVICE_MIG_DISABLE;
        *pendingMode = NVML_DEVICE_MIG_DISABLE;
        return NVML_SUCCESS;
    }
    return ret;
}

DEF_FN(nvmlReturn_t, nvmlSystemGetTopologyGpuSet, unsigned int,  cpuNumber, unsigned int*, count, nvmlDevice_t*, deviceArray )
DEF_FN(nvmlReturn_t, nvmlVgpuInstanceGetMdevUUID, nvmlVgpuInstance_t, vgpuInstance, char*, mdevUuid, unsigned int,  size )
