#pragma once
#include <spdlog/spdlog.h>


// Useful for forwarding member to_json to derived types through pointer types


#define WMTK_TRANSFER_STRATEGY_PARAMETERS_CLONE(NAME)     \
    std::unique_ptr<TransferStrategy> NAME::clone() const \
    {                                                     \
        return std::make_unique<NAME>(*this);             \
    }

#define WMTK_TRANSFER_STRATEGY_PARAMETERS_CLONE_TEMPLATE(NAME) \
    template <>                                                \
    std::unique_ptr<TransferStrategy> NAME::clone() const      \
    {                                                          \
        return std::make_unique<NAME>(*this);                  \
    }

#define WMTK_TRANSFER_STRATEGY_REGISTER_SELF(CLASS_NAME, NAME)  \
    std::nullptr_t CLASS_NAME::register_self()                  \
    {                                                           \
        TransferStrategy::register_transfer<CLASS_NAME>(#NAME); \
        return nullptr;                                         \
    }                                                           \
    namespace {                                                 \
    std::nullptr_t _ = CLASS_NAME::register_self();             \
    }

#define WMTK_TRANSFER_ALL_DEFINITIONS(CLASS_NAME, NAME) \
    WMTK_TRANSFER_STRATEGY_PARAMETERS_CLONE(CLASS_NAME)
#define WMTK_TRANSFER_ALL_DEFINITIONS_TEMPLATE(CLASS_NAME, NAME)        \
    WMTK_TRANSFER_STRATEGY_PARAMETERS_JSON_FORWARD_TEMPLATE(CLASS_NAME) \
    WMTK_TRANSFER_STRATEGY_PARAMETERS_CLONE_TEMPLATE(                   \
        CLASS_NAME) // WMTK_TRANSFER_STRATEGY_REGISTER_SELF(CLASS_NAME, NAME)

