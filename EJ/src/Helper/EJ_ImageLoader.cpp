#include "EJ/Helper/EJ_ImageLoader.hpp"

#include <stb_image/stb_image.h>

namespace {
	struct foo {};
}

EJ_NAMESPACE_BEGIN

_STD optional<SimpleImage> ImageLoader::stbiLoad(const _STD filesystem::path& filename_, int desiredChannel_, bool flipVertically_) {
	SimpleImage ret;
	stbi_set_flip_vertically_on_load(flipVertically_);
	_STD u8string filenameu8 = filename_.u8string();
	_STD string filenameStr{ filenameu8.begin(), filenameu8.end() };
	ret.data = _STD shared_ptr<void>(stbi_load(filenameStr.c_str(), &ret.width, &ret.height, &ret.channel, desiredChannel_), stbi_image_free);
	if (ret.data)
	{
		if (desiredChannel_ != 0) {
			ret.channel = desiredChannel_;
		}
		return ret;
	}
	return _STD nullopt;
}

EJ_NAMESPACE_END

