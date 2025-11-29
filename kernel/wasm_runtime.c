#include "wasm_runtime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RUNTIMES 64
#define WASM_MAGIC 0x6d736100

static WasmRuntime g_runtimes[MAX_RUNTIMES] = {0};
static uint32_t g_runtime_count = 0;
static uint32_t g_next_runtime_id = 1;

typedef struct {
    uint32_t runtime_id;
    void* memory;
    uint32_t memory_pages;
} WasmMemory;

static WasmMemory g_memories[MAX_RUNTIMES] = {0};

typedef struct {
    uint32_t runtime_id;
    WasmFunction functions[256];
    uint32_t function_count;
} WasmModuleData;

static WasmModuleData g_modules[MAX_RUNTIMES] = {0};

int wasm_runtime_init(void) {
    memset(g_runtimes, 0, sizeof(g_runtimes));
    memset(g_memories, 0, sizeof(g_memories));
    memset(g_modules, 0, sizeof(g_modules));
    return 0;
}

int wasm_runtime_create(uint32_t* runtime_id, WasmModule* module) {
    if (!runtime_id || !module || g_runtime_count >= MAX_RUNTIMES) {
        return -1;
    }
    
    *runtime_id = g_next_runtime_id++;
    
    WasmRuntime* runtime = &g_runtimes[g_runtime_count];
    runtime->runtime_id = *runtime_id;
    runtime->state = WASM_STATE_IDLE;
    strncpy(runtime->module_name, module->module_name, sizeof(runtime->module_name) - 1);
    runtime->memory_pages = 1;
    runtime->memory_usage = 65536;
    runtime->function_count = 0;
    
    WasmMemory* mem = &g_memories[g_runtime_count];
    mem->runtime_id = *runtime_id;
    mem->memory_pages = 1;
    mem->memory = malloc(65536);
    
    WasmModuleData* mod_data = &g_modules[g_runtime_count];
    mod_data->runtime_id = *runtime_id;
    mod_data->function_count = 0;
    
    g_runtime_count++;
    return 0;
}

int wasm_runtime_load_module(uint32_t runtime_id, const char* module_path) {
    if (runtime_id == 0 || !module_path) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            g_runtimes[i].state = WASM_STATE_LOADED;
            g_runtimes[i].function_count = 5;
            return 0;
        }
    }
    
    return -1;
}

int wasm_runtime_get_function(uint32_t runtime_id, const char* func_name, WasmFunction* func) {
    if (runtime_id == 0 || !func_name || !func) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            WasmModuleData* mod = &g_modules[i];
            
            for (uint32_t j = 0; j < mod->function_count; j++) {
                if (strcmp(mod->functions[j].name, func_name) == 0) {
                    *func = mod->functions[j];
                    return 0;
                }
            }
            
            return -1;
        }
    }
    
    return -1;
}

int wasm_call_function(uint32_t runtime_id, const char* func_name, WasmValue* args, uint32_t arg_count, WasmValue* result) {
    if (runtime_id == 0 || !func_name) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            if (g_runtimes[i].state != WASM_STATE_LOADED && g_runtimes[i].state != WASM_STATE_RUNNING) {
                return -1;
            }
            
            g_runtimes[i].state = WASM_STATE_RUNNING;
            
            if (result) {
                result->i64 = 0;
            }
            
            g_runtimes[i].state = WASM_STATE_LOADED;
            return 0;
        }
    }
    
    return -1;
}

int wasm_runtime_get_memory(uint32_t runtime_id, uint32_t offset, void* buffer, size_t size) {
    if (runtime_id == 0 || !buffer || size == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_memories[i].runtime_id == runtime_id) {
            if (offset + size > g_memories[i].memory_pages * 65536) {
                return -1;
            }
            
            if (g_memories[i].memory) {
                memcpy(buffer, (uint8_t*)g_memories[i].memory + offset, size);
                return 0;
            }
        }
    }
    
    return -1;
}

int wasm_runtime_set_memory(uint32_t runtime_id, uint32_t offset, const void* data, size_t size) {
    if (runtime_id == 0 || !data || size == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_memories[i].runtime_id == runtime_id) {
            if (offset + size > g_memories[i].memory_pages * 65536) {
                return -1;
            }
            
            if (g_memories[i].memory) {
                memcpy((uint8_t*)g_memories[i].memory + offset, data, size);
                return 0;
            }
        }
    }
    
    return -1;
}

int wasm_runtime_get_state(uint32_t runtime_id) {
    if (runtime_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            return g_runtimes[i].state;
        }
    }
    
    return -1;
}

int wasm_runtime_suspend(uint32_t runtime_id) {
    if (runtime_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            if (g_runtimes[i].state == WASM_STATE_RUNNING) {
                g_runtimes[i].state = WASM_STATE_SUSPENDED;
                return 0;
            }
        }
    }
    
    return -1;
}

int wasm_runtime_resume(uint32_t runtime_id) {
    if (runtime_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            if (g_runtimes[i].state == WASM_STATE_SUSPENDED) {
                g_runtimes[i].state = WASM_STATE_RUNNING;
                return 0;
            }
        }
    }
    
    return -1;
}

int wasm_runtime_destroy(uint32_t runtime_id) {
    if (runtime_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            if (g_memories[i].memory) {
                free(g_memories[i].memory);
                g_memories[i].memory = NULL;
            }
            
            g_runtimes[i].runtime_id = 0;
            g_runtimes[i].state = WASM_STATE_IDLE;
            return 0;
        }
    }
    
    return -1;
}

int wasm_validate_module(const void* module_data, size_t size) {
    if (!module_data || size == 0) {
        return -1;
    }
    
    uint32_t* magic = (uint32_t*)module_data;
    return *magic == WASM_MAGIC ? 0 : -1;
}

int wasm_runtime_get_info(uint32_t runtime_id, WasmRuntime* info) {
    if (runtime_id == 0 || !info) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_runtime_count; i++) {
        if (g_runtimes[i].runtime_id == runtime_id) {
            *info = g_runtimes[i];
            return 0;
        }
    }
    
    return -1;
}
