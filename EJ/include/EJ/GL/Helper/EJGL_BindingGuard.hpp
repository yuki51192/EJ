#ifndef EJGL_BINDINGGUARD_HPP
#define EJGL_BINDINGGUARD_HPP

#include "../Utils/EJGL_config.hpp"

EJGL_NAMESPACE_BEGIN

template <typename _ResourceType>
class BindingGuard
{
public:
	using ResourceType = _ResourceType;

public:
	BindingGuard(const ResourceType& resource_) :
		_resource(resource_)
	{
		_resource.bind();
	}

	~BindingGuard() {
		_resource.unbind();
	}

	BindingGuard(const BindingGuard& obj_) = delete;
	BindingGuard& operator=(const BindingGuard& obj_) = delete;

private:
	const ResourceType& _resource;

};

EJGL_NAMESPACE_END

#endif // EJGL_BINDINGGUARD_HPP