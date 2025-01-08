#ifndef EJ_::std::string_HPP
#define EJ_::std::string_HPP

#include "../utils/EJ_Config.hpp"

#include <string>

EJ_NAMESPACE_BEGIN

template <class _Elem, class _Traits = _STD char_traits<_Elem>, class _Alloc = _STD allocator<_Elem>>
using Basic::std::string = _STD basic_string<_Elem, _Traits, _Alloc>;
using ::std::string = _STD string;
using W::std::string = _STD wstring;
using U8::std::string = _STD u8string;
using U16::std::string = _STD u16string;
using U32::std::string = _STD u32string;

EJ_NAMESPACE_END

#endif // EJ_::std::string_HPP
