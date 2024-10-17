#pragma once

#include <kernel.hpp>

namespace kernel
{
    struct SourceLocation
    {
        const char *file;
        uint32_t line;
        uint32_t column;
    };
    
    struct TypeDescriptor
    {
        uint16_t kind;
        uint16_t info;
        char name[];
    };

    struct InvalidValueData
    {
        SourceLocation location;
        TypeDescriptor const& type;
    };

    struct NonnullArgData
    {
        SourceLocation location;
        SourceLocation attribute_location;
        int argument_index;
    };

    struct NonnullReturnData
    {
        SourceLocation attribute_location;
    };

    struct OverflowData
    {
        SourceLocation location;
        TypeDescriptor const& type;
    };

    struct VLABoundData
    {
        SourceLocation location;
        TypeDescriptor const& type;
    };

    struct ShiftOutOfBoundsData
    {
        SourceLocation location;
        TypeDescriptor const& lhs_type;
        TypeDescriptor const& rhs_type;
    };

    struct OutOfBoundsData
    {
        SourceLocation location;
        TypeDescriptor const& array_type;
        TypeDescriptor const& index_type;
    };

    struct TypeMismatchData
    {
        SourceLocation location;
        TypeDescriptor const& type;
        u8 log_alignment;
        u8 type_check_kind;
    };

    struct AlignmentAssumptionData
    {
        SourceLocation location;
        SourceLocation assumption_location;
        TypeDescriptor const& type;
    };

    struct UnreachableData
    {
        SourceLocation location;
    };

    struct ImplicitConversionData
    {
        SourceLocation location;
        TypeDescriptor const& from_type;
        TypeDescriptor const& to_type;
        /* ImplicitConversionCheckKind */ unsigned char kind;
    };

    struct InvalidBuiltinData
    {
        SourceLocation location;
        unsigned char kind;
    };

    struct PointerOverflowData
    {
        SourceLocation location;
    };

    struct FunctionTypeMismatchData
    {
        SourceLocation location;
        TypeDescriptor const& type;
    };

    struct FloatCastOverflowData
    {
        SourceLocation location;
        TypeDescriptor const& from_type;
        TypeDescriptor const& to_type;
    };
}