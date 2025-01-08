#include "EJ/GL/GlObject/EJGL_BufferObject.hpp"
#include <string>

EJGL_NAMESPACE_BEGIN

#pragma region BufferObject

class BufferObject::_BufferObjectImpl {
public:
	_BufferObjectImpl(BufferType type_) :
		_bufferType(type_)
	{}
	_BufferObjectImpl(BufferType type_, GLsizeiptr size_, const void* data_ = nullptr, DataUsage usage_ = DataUsage::STATIC_DRAW) noexcept :
		_bufferType(type_)
	{
		create(size_, data_, usage_);
	}
	_BufferObjectImpl(GLuint id_, BufferType type_) :
		_bufferID(id_), _bufferType(type_)
	{}
	_BufferObjectImpl(const _BufferObjectImpl& obj_) = delete;
	_BufferObjectImpl(_BufferObjectImpl&& obj_) noexcept = delete;
	_BufferObjectImpl& operator=(const _BufferObjectImpl& obj_) = delete;
	_BufferObjectImpl& operator=(_BufferObjectImpl&& obj_) noexcept = delete;
	~_BufferObjectImpl() noexcept {
		deleteBuffer();
	}

	_STD string toStringNoHeader() const noexcept {
		return "{ ID: " + _EJ toString(_bufferID) + " }";
	}

	void bind() const noexcept {
		glBindBuffer(_bufferType, _bufferID);
	}

	void create(GLsizeiptr size_, const void* data_, DataUsage usage_) noexcept {
		glGenBuffers(1, &_bufferID);
		bind();
		glBufferData(_bufferType, size_, data_, usage_);
	}

	void subData(GLintptr offset_, GLsizeiptr size_, const void* data_) const noexcept {
		glBufferSubData(_bufferType, offset_, size_, data_);
	}

	void deleteBuffer() noexcept {
		glDeleteBuffers(1, &_bufferID);
		_bufferID = 0;
	}

public:
	GLuint _bufferID = 0;
	BufferType _bufferType = BufferType::ARRAY;

public:
	static void unbind(BufferType bufferType_) noexcept {
		glBindBuffer(bufferType_, 0);
	}

};

BufferObject::BufferObject(BufferType type_) noexcept :
	_impl{ _STD make_shared<_BufferObjectImpl>(type_) }
{}
BufferObject::BufferObject(BufferType type_, GLsizeiptr size_, const void* data_, DataUsage usage_) noexcept :
	_impl{ _STD make_shared<_BufferObjectImpl>(type_, size_, data_, usage_) }
{}
BufferObject::BufferObject(GLuint id_, BufferType type_) noexcept :
	_impl{ _STD make_shared<_BufferObjectImpl>(type_, id_) }
{}

_STD string BufferObject::toStringNoHeader() const noexcept {
	return _impl->toStringNoHeader();
}
_STD string BufferObject::toString() const noexcept {
	return _STD string("[EJ][BufferObject]") + _impl->toStringNoHeader();
}

void BufferObject::swap(BufferObject& obj_) noexcept {
	_STD swap(_impl, obj_._impl);
}

BufferObject::operator GLuint() const {
	return _impl->_bufferID;
}
bool BufferObject::isValid() const {
	return _impl->_bufferID != 0;
}
GLuint BufferObject::getBufferID() const {
	return _impl->_bufferID;
}
BufferType BufferObject::getBufferType() const {
	return _impl->_bufferType;
}

void BufferObject::setBufferID(GLuint ID_) noexcept {
	_impl->_bufferID = ID_;
}
void BufferObject::setBufferType(BufferType bufferType_) noexcept {
	_impl->_bufferType = bufferType_;
}

void BufferObject::bind() const noexcept {
	EJ_ASSERT(isValid() && "Have you create()?");
	return _impl->bind();
}
void BufferObject::unbind() const noexcept {
	_BufferObjectImpl::unbind(_impl->_bufferType);
}

void BufferObject::create(GLsizeiptr size_, const void* data_, DataUsage usage_) noexcept {
	return _impl->create(size_, data_, usage_);
}

void BufferObject::subData(GLintptr offset_, GLsizeiptr size_, const void* data_) const noexcept {
	EJ_ASSERT(isValid() && "Have you create()?");
	return _impl->subData(offset_, size_, data_);
}

void BufferObject::deleteBuffer() noexcept {
	return _impl->deleteBuffer();
}

void BufferObject::unbind(BufferType bufferType_) noexcept {
	_BufferObjectImpl::unbind(bufferType_);
}

#pragma endregion

#pragma region ArrayBuffer

#pragma endregion

#pragma region ElementBuffer

#pragma endregion

#pragma region UniformBuffer

void UniformBuffer::bindToBindingPoint(GLuint bindingPoint_) noexcept {
	EJ_ASSERT(isValid() && "Have you create()?");
	_bindingPoint = bindingPoint_;
	glBindBufferBase(BufferType::UNIFORM, bindingPoint_, getBufferID());
}
void UniformBuffer::bindToBindingPoint(GLuint bindingPoint_, GLintptr offset_, GLsizeiptr size_) noexcept {
	EJ_ASSERT(isValid() && "Have you create()?");
	_bindingPoint = bindingPoint_;
	glBindBufferRange(BufferType::UNIFORM, bindingPoint_, getBufferID(), offset_, size_);
}

#pragma endregion

EJGL_NAMESPACE_END