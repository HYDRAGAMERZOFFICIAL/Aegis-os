#ifndef WASM_RUNTIME_H
#define WASM_RUNTIME_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    WASM_TYPE_I32 = 0,
    WASM_TYPE_I64 = 1,
    WASM_TYPE_F32 = 2,
    WASM_TYPE_F64 = 3,
    WASM_TYPE_V128 = 4,
} WasmType;

typedef enum {
    WASM_STATE_IDLE = 0,
    WASM_STATE_LOADED = 1,
    WASM_STATE_RUNNING = 2,
    WASM_STATE_SUSPENDED = 3,
    WASM_STATE_FAILED = 4,
} WasmState;

typedef union {
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;
} WasmValue;

typedef struct {
    char name[64];
    WasmType param_types[16];
    uint32_t param_count;
    WasmType return_type;
} WasmFunction;

typedef struct {
    uint32_t runtime_id;
    WasmState state;
    char module_name[64];
    uint32_t memory_pages;
    uint64_t memory_usage;
    uint32_t function_count;
} WasmRuntime;

typedef struct {
    void* module_data;
    size_t module_size;
    char module_name[64];
} WasmModule;

int wasm_runtime_init(void);
int wasm_runtime_create(uint32_t* runtime_id, WasmModule* module);
int wasm_runtime_load_module(uint32_t runtime_id, const char* module_path);
int wasm_runtime_get_function(uint32_t runtime_id, const char* func_name, WasmFunction* func);
int wasm_call_function(uint32_t runtime_id, const char* func_name, WasmValue* args, uint32_t arg_count, WasmValue* result);
int wasm_runtime_get_memory(uint32_t runtime_id, uint32_t offset, void* buffer, size_t size);
int wasm_runtime_set_memory(uint32_t runtime_id, uint32_t offset, const void* data, size_t size);
int wasm_runtime_get_state(uint32_t runtime_id);
int wasm_runtime_suspend(uint32_t runtime_id);
int wasm_runtime_resume(uint32_t runtime_id);
int wasm_runtime_destroy(uint32_t runtime_id);
int wasm_validate_module(const void* module_data, size_t size);
int wasm_runtime_get_info(uint32_t runtime_id, WasmRuntime* info);

#endif
