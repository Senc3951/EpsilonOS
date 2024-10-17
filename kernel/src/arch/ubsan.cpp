#include <arch/ubsan.hpp>

#ifdef __cplusplus
extern "C" {
#endif

namespace kernel
{
using ValueHandle = void*;

static void print_location(SourceLocation const& location, const char *s)
{
    panic("%s in file %s, line %u, column %u", s, location.file, location.line, location.column);
}

void __ubsan_handle_load_invalid_value(InvalidValueData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_load_invalid_value(InvalidValueData const& data, ValueHandle)
{
    print_location(data.location, "load invalid value");
}

void __ubsan_handle_nonnull_arg(NonnullArgData const&) __attribute__((used));
void __ubsan_handle_nonnull_arg(NonnullArgData const& data)
{
    print_location(data.location, "nonull arg");
}

void __ubsan_handle_nullability_arg(NonnullArgData const&) __attribute__((used));
void __ubsan_handle_nullability_arg(NonnullArgData const& data)
{
    print_location(data.location, "nullability arg");
}

void __ubsan_handle_nonnull_return_v1(NonnullReturnData const&, SourceLocation const&) __attribute__((used));
void __ubsan_handle_nonnull_return_v1(NonnullReturnData const&, SourceLocation const& location)
{
    print_location(location, "nonull return");
}

void __ubsan_handle_nullability_return_v1(NonnullReturnData const& data, SourceLocation const& location) __attribute__((used));
void __ubsan_handle_nullability_return_v1(NonnullReturnData const&, SourceLocation const& location)
{
    print_location(location, "nullability return");
}

void __ubsan_handle_vla_bound_not_positive(VLABoundData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_vla_bound_not_positive(VLABoundData const& data, ValueHandle)
{
    print_location(data.location, "bound not positive");
}

void __ubsan_handle_add_overflow(OverflowData const&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_add_overflow(OverflowData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "add overflow");
}

void __ubsan_handle_sub_overflow(OverflowData const&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_sub_overflow(OverflowData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "sub overflow");
}

void __ubsan_handle_negate_overflow(OverflowData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_negate_overflow(OverflowData const& data, ValueHandle)
{
    print_location(data.location, "negate overflow");
}

void __ubsan_handle_mul_overflow(OverflowData const&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_mul_overflow(OverflowData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "mul overflow");
}

void __ubsan_handle_shift_out_of_bounds(ShiftOutOfBoundsData const&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_shift_out_of_bounds(ShiftOutOfBoundsData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "shift out of bounds");
}

void __ubsan_handle_divrem_overflow(OverflowData const&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_divrem_overflow(OverflowData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "divrem overflow");
}

void __ubsan_handle_out_of_bounds(OutOfBoundsData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_out_of_bounds(OutOfBoundsData const& data, ValueHandle)
{
    print_location(data.location, "out of bounds");
}

void __ubsan_handle_type_mismatch_v1(TypeMismatchData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_type_mismatch_v1(TypeMismatchData const& data, ValueHandle)
{
    print_location(data.location, "type mismatch");
}

void __ubsan_handle_alignment_assumption(AlignmentAssumptionData const&, ValueHandle, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_alignment_assumption(AlignmentAssumptionData const& data, ValueHandle, ValueHandle, ValueHandle)
{
    print_location(data.location, "alignment assumption");
}

void __ubsan_handle_builtin_unreachable(UnreachableData const&) __attribute__((used));
void __ubsan_handle_builtin_unreachable(UnreachableData const& data)
{
    print_location(data.location, "unreachable");
}

void __ubsan_handle_missing_return(UnreachableData const&) __attribute__((used));
void __ubsan_handle_missing_return(UnreachableData const& data)
{
    print_location(data.location, "missing return");
}

void __ubsan_handle_implicit_conversion(ImplicitConversionData const&, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_implicit_conversion(ImplicitConversionData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "implicit conversion");
}

void __ubsan_handle_invalid_builtin(InvalidBuiltinData const) __attribute__((used));
void __ubsan_handle_invalid_builtin(InvalidBuiltinData const data)
{
    print_location(data.location, "invalid builtin");
}

void __ubsan_handle_pointer_overflow(PointerOverflowData const&, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_pointer_overflow(PointerOverflowData const& data, ValueHandle, ValueHandle)
{
    print_location(data.location, "pointer overflow");
}

void __ubsan_handle_function_type_mismatch(FunctionTypeMismatchData const&, ValueHandle) __attribute__((used));
void __ubsan_handle_function_type_mismatch(FunctionTypeMismatchData const& data, ValueHandle)
{
    print_location(data.location, "function type mismatch");
}
}

#ifdef __cplusplus
};
#endif