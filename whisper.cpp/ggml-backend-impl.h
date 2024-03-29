#pragma once

// ggml-backend internal header

#include "ggml-backend.h"

#ifdef  __cplusplus
extern "C" {
#endif

    //
    // Backend buffer
    //

    // buffer type
    typedef void * ggml_backend_buffer_type_context_t;

    struct ggml_backend_buffer_type_i {
        const char *          (*get_name)        (ggml_backend_buffer_type_t buft);
        ggml_backend_buffer_t (*alloc_buffer)    (ggml_backend_buffer_type_t buft, size_t size);
        size_t                (*get_alignment)   (ggml_backend_buffer_type_t buft); // tensor alignment
        size_t                (*get_alloc_size)  (ggml_backend_buffer_type_t buft, const struct ggml_tensor * tensor); // data size needed to allocate the tensor, including padding
        bool                  (*supports_backend)(ggml_backend_buffer_type_t buft, ggml_backend_t backend); // check if the buffer type is usable by the backend
        // check if tensor data is in host memory
        // should be equivalent to supports_backend(buft, ggml_backend_cpu_init())
        bool                  (*is_host)         (ggml_backend_buffer_type_t buft);
    };

    struct ggml_backend_buffer_type {
        struct ggml_backend_buffer_type_i  iface;
        ggml_backend_buffer_type_context_t context;
    };

    // buffer
    typedef void * ggml_backend_buffer_context_t;

    struct ggml_backend_buffer_i {
        const char * (*get_name)   (ggml_backend_buffer_t buffer);
        void         (*free_buffer)(ggml_backend_buffer_t buffer);
        void *       (*get_base)   (ggml_backend_buffer_t buffer);
        void         (*init_tensor)(ggml_backend_buffer_t buffer, struct ggml_tensor * tensor);
        void         (*set_tensor) (ggml_backend_buffer_t buffer,       struct ggml_tensor * tensor, const void * data, size_t offset, size_t size);
        void         (*get_tensor) (ggml_backend_buffer_t buffer, const struct ggml_tensor * tensor,       void * data, size_t offset, size_t size);
        bool         (*cpy_tensor) (ggml_backend_buffer_t buffer, const struct ggml_tensor * src, struct ggml_tensor * dst); // dst is in the buffer, src may be in any buffer
        void         (*clear)      (ggml_backend_buffer_t buffer, uint8_t value);
        void         (*reset)      (ggml_backend_buffer_t buffer); // reset any internal state due to tensor initialization, such as tensor extras
    };

    struct ggml_backend_buffer {
        struct ggml_backend_buffer_i  iface;
        ggml_backend_buffer_type_t    buft;
        ggml_backend_buffer_context_t context;
        size_t size;
        enum ggml_backend_buffer_usage usage;
    };

    ggml_backend_buffer_t ggml_backend_buffer_init(
                   ggml_backend_buffer_type_t      buft,
            struct ggml_backend_buffer_i           iface,
                   ggml_backend_buffer_context_t   context,
                   size_t                          size);

    // do not use directly, use ggml_backend_tensor_copy instead
    bool ggml_backend_buffer_copy_tensor(const struct ggml_tensor * src, struct ggml_tensor * dst);

    //
    // Backend
    //

    typedef void * ggml_backend_context_t;

    struct ggml_backend_i {
        const char * (*get_name)(ggml_backend_t backend);

        void (*free)(ggml_backend_t backend);

        // buffer allocation
        ggml_backend_buffer_type_t (*get_default_buffer_type)(ggml_backend_t backend);

        // (optional) asynchronous tensor data access
        void (*set_tensor_async)(ggml_backend_t backend,       struct ggml_tensor * tensor, const void * data, size_t offset, size_t size);
        void (*get_tensor_async)(ggml_backend_t backend, const struct ggml_tensor * tensor,       void * data, size_t offset, size_t size);
        bool (*cpy_tensor_async)(ggml_backend_t backend, const struct ggml_tensor * src, struct ggml_tensor * dst);

        // (optional) complete all pending operations
        void (*synchronize)(ggml_backend_t backend);

        // compute graph with a plan
        ggml_backend_graph_plan_t (*graph_plan_create) (ggml_backend_t backend, const struct ggml_cgraph * cgraph);
        void                      (*graph_plan_free)   (ggml_backend_t backend, ggml_backend_graph_plan_t plan);
        void                      (*graph_plan_compute)(ggml_backend_t backend, ggml_backend_graph_plan_t plan);

        // compute graph without a plan (async)
        bool (*graph_compute)(ggml_backend_t backend, struct ggml_cgraph * cgraph);

        // check if the backend supports an operation
        bool (*supports_op)(ggml_backend_t backend, const struct ggml_tensor * op);
    };

    struct ggml_backend {
        struct ggml_backend_i iface;

        ggml_backend_context_t context;
    };

    //
    // Backend registry
    //

    typedef ggml_backend_t (*ggml_backend_init_fn)(const char * params, void * user_data);

    void ggml_backend_register(const char * name, ggml_backend_init_fn init_fn, ggml_backend_buffer_type_t default_buffer_type, void * user_data);

#ifdef  __cplusplus
}
#endif
