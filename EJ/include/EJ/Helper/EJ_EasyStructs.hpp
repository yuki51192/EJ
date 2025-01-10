#ifndef EJ_EASYSTRUCTS_HPP
#define EJ_EASYSTRUCTS_HPP

#include "../Utils/EJ_config.hpp"

#include <memory>

EJ_NAMESPACE_BEGIN

struct SimpleImage {
	int width;
	int height;
	int channel;
	_STD shared_ptr<void> data;
};

EJ_NAMESPACE_END

#endif // EJ_EASYSTRUCTS_HPP