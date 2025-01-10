#ifndef EJ_IMAGELOADER_HPP
#define EJ_IMAGELOADER_HPP

#include "../Utils/EJ_config.hpp"
#include "EJ_EasyStructs.hpp"

#include <filesystem>
#include <optional>

EJ_NAMESPACE_BEGIN

class ImageLoader {
public:
	static _STD optional<SimpleImage> stbiLoad(const _STD filesystem::path& filename_, int desiredChannel_ = 0, bool flipVertical_ = true);
};

EJ_NAMESPACE_END

#endif // EJ_IMAGELOADER_HPP