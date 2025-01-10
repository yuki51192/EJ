#ifndef EJ_MACROS_HPP
#define EJ_MACROS_HPP

#include "EJ_config.hpp"

EJ_NAMESPACE_BEGIN

#define EJ_M_EMPTY_PARSE_WITH_ERROR_FUNC() \
	constexpr auto parse(_STD format_parse_context& ctx) { \
		static_assert(ctx.begin() == ctx.end()); \
		return ctx.begin(); \
	}

#define EJ_M_GET_FUNC(retT, funcN, retV) \
	retT funcN() noexcept { return retV; }
#define EJ_M_CONST_GET_FUNC(retT, funcN, retV) \
	retT funcN() const noexcept { return retV; }
#define EJ_M_SET_FUNC(valT, funcN, exp) \
	void funcN(valT val_) noexcept { exp; }

EJ_NAMESPACE_END

#endif // EJ_MACROS_HPP
