#ifndef EJGL_BUFFEROBJECT_HPP
#define EJGL_BUFFEROBJECT_HPP

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../../Utils/EJ_Utils.hpp"
#include "../Utils/EJGL_enum.hpp"

#include <memory>
#include <span>

EJGL_NAMESPACE_BEGIN

class BufferObject
{
private:
	class _BufferObjectImpl;

public:
	// construct empty
	BufferObject(BufferType type_ = BufferType::ARRAY) noexcept;
	// call create() after construct
	BufferObject(BufferType type_, GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept;
	// call create() after construct
	template<typename Type_>
	BufferObject(BufferType type_, _STD span<Type_>data_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(type_, data_.size() * sizeof(Type_), data_.data(), usage_)
	{}
	template <_STD ranges::contiguous_range Range_>
		requires _STD ranges::sized_range<Range_>
	BufferObject(BufferType type_, Range_&& range_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(type_, _STD span<const _STD ranges::range_value_t<Range_>>(range_), usage_)
	{}
	// construct with id type
	BufferObject(GLuint id_, BufferType type_) noexcept;
	BufferObject(const BufferObject& obj_) noexcept = default;
	BufferObject& operator=(const BufferObject& obj_) noexcept = default;
	virtual ~BufferObject() noexcept = default;

	_STD string toStringNoHeader() const noexcept;
	_STD string toString() const noexcept;

	void swap(BufferObject& obj_) noexcept;

	operator GLuint() const;
	bool isValid() const;
	GLuint getBufferID() const;
	BufferType getBufferType() const;

	void setBufferID(GLuint ID_) noexcept;
	void setBufferType(BufferType bufferType_) noexcept;

	void bind() const noexcept;
	void unbind() const noexcept;

	void create(GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept;
	template<typename Type_>
	void create(_STD span<Type_> data_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept {
		create(data_.size() * sizeof(Type_), data_.data(), usage_);
	}
	template <_STD ranges::contiguous_range Range_>
		requires _STD ranges::sized_range<Range_>
	void create(Range_&& range_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept {
		create(_STD span<const _STD ranges::range_value_t<Range_>>(range_), usage_);
	}

	void subData(GLintptr offset_, GLsizeiptr size_, const void* data_) const noexcept;

	void deleteBuffer() noexcept;

private:
	_STD shared_ptr<_BufferObjectImpl> _impl;

	// static
public:
	static void unbind(BufferType bufferType_) noexcept;

};

class ArrayBuffer : public BufferObject
{
public:
	// construct empty
	ArrayBuffer() noexcept :
		BufferObject(BufferType::ARRAY)
	{}
	// call create() after construct
	ArrayBuffer(GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::ARRAY, size_, data_, usage_)
	{}
	// call create() after construct
	template<typename Type_>
	ArrayBuffer(_STD span<Type_>data_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::ARRAY, data_.size() * sizeof(Type_), data_.data(), usage_)
	{}
	template <_STD ranges::contiguous_range Range_>
		requires _STD ranges::sized_range<Range_>
	ArrayBuffer(Range_&& range_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		ArrayBuffer(_STD span<const _STD ranges::range_value_t<Range_>>(range_), usage_)
	{}
	// construct with id type
	ArrayBuffer(GLuint id_) noexcept :
		BufferObject(id_, BufferType::ARRAY)
	{}
	ArrayBuffer(const ArrayBuffer& obj_) noexcept = default;
	ArrayBuffer& operator=(const ArrayBuffer& obj_) noexcept = default;
	virtual ~ArrayBuffer() noexcept = default;

	_STD string toStringNoHeader() const noexcept {
		return BufferObject::toStringNoHeader();
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][ArrayBuffer]") + toStringNoHeader();
	}

private:

public:
	static void unbind() noexcept {
		BufferObject::unbind(BufferType::ARRAY);
	}

};

class ElementBuffer : public BufferObject
{
public:
	// construct empty
	ElementBuffer() noexcept :
		BufferObject(BufferType::ELEMENT)
	{}
	// call create() after construct
	ElementBuffer(GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::ELEMENT, size_, data_, usage_)
	{}
	// call create() after construct
	template<typename Type_>
	ElementBuffer(_STD span<Type_> data_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::ELEMENT, data_.size() * sizeof(Type_), data_.data(), usage_)
	{}
	template <_STD ranges::contiguous_range Range_>
		requires _STD ranges::sized_range<Range_>
	ElementBuffer(Range_&& range_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		ElementBuffer(_STD span<const _STD ranges::range_value_t<Range_>>(range_), usage_)
	{}
	// construct with id type
	ElementBuffer(GLuint id_) noexcept :
		BufferObject(id_, BufferType::ELEMENT)
	{}
	ElementBuffer(const ElementBuffer& obj_) noexcept = default;
	ElementBuffer& operator=(const ElementBuffer& obj_) noexcept = default;
	virtual ~ElementBuffer() noexcept = default;

	_STD string toStringNoHeader() const noexcept {
		return BufferObject::toStringNoHeader();
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][ElementBuffer]") + toStringNoHeader();
	}

private:

public:
	static void unbind() noexcept {
		BufferObject::unbind(BufferType::ELEMENT);
	}

};

class UniformBuffer : public BufferObject
{
public:
	// construct empty
	UniformBuffer() noexcept :
		BufferObject(BufferType::UNIFORM)
	{}
	// call create() after construct
	UniformBuffer(GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::UNIFORM, size_, data_, usage_)
	{}
	// call create() after construct
	template<typename Type_>
	UniformBuffer(_STD span<Type_> data_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		BufferObject(BufferType::UNIFORM, data_.size() * sizeof(Type_), data_.data(), usage_)
	{}
	template <_STD ranges::contiguous_range Range_>
		requires _STD ranges::sized_range<Range_>
	UniformBuffer(Range_&& range_, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		UniformBuffer(_STD span<const _STD ranges::range_value_t<Range_>>(range_), usage_)
	{}
	// construct with id type
	UniformBuffer(GLuint id_, GLuint bindingPoint_ = 0) noexcept :
		BufferObject(id_, BufferType::UNIFORM),
		_bindingPoint(bindingPoint_)
	{}
	UniformBuffer(const UniformBuffer& obj_) noexcept = default;
	UniformBuffer& operator=(const UniformBuffer& obj_) noexcept = default;
	virtual ~UniformBuffer() noexcept = default;

	_STD string toStringNoHeader() const noexcept {
		return BufferObject::toStringNoHeader();
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][ElementBuffer]") + toStringNoHeader();
	}

	EJ_M_CONST_GET_FUNC(GLuint, getBindingPoint, _bindingPoint);

	// use glBindBufferBase
	void bindToBindingPoint(GLuint bindingPoint_) noexcept;
	// use glBindBufferRange
	void bindToBindingPoint(GLuint bindingPoint_, GLintptr offset_, GLsizeiptr size_) noexcept;

private:
	GLuint _bindingPoint = 0;

public:
	static void unbind() noexcept {
		BufferObject::unbind(BufferType::UNIFORM);
	}

};

EJGL_NAMESPACE_END

#endif // EJGL_BUFFEROBJECT_HPP