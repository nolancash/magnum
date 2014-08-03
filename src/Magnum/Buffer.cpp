/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Buffer.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/State.h"
#include "Implementation/BufferState.h"
#include "Implementation/DebugState.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES
Int Buffer::minMapAlignment() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::map_buffer_alignment>())
        return 0;

    GLint& value = Context::current()->state().buffer->minMapAlignment;

    if(value == 0)
        glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &value);

    return value;
}

Int Buffer::maxAtomicCounterBindings() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>())
        return 0;

    GLint& value = Context::current()->state().buffer->maxAtomicCounterBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &value);

    return value;
}

Int Buffer::maxShaderStorageBindings() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>())
        return 0;

    GLint& value = Context::current()->state().buffer->maxShaderStorageBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &value);

    return value;
}

Int Buffer::shaderStorageOffsetAlignment() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>())
        return 0;

    GLint& value = Context::current()->state().buffer->shaderStorageOffsetAlignment;

    if(value == 0)
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Int Buffer::maxUniformBindings() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current()->state().buffer->maxUniformBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Buffer::copy(Buffer& read, Buffer& write, const GLintptr readOffset, const GLintptr writeOffset, const GLsizeiptr size) {
    Context::current()->state().buffer->copyImplementation(read, write, readOffset, writeOffset, size);
}
#endif

Buffer::Buffer(Buffer::Target targetHint): _targetHint(targetHint)
    #ifdef CORRADE_TARGET_NACL
    , _mappedBuffer(nullptr)
    #endif
{
    glGenBuffers(1, &_id);
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

Buffer::~Buffer() {
    /* Moved out, nothing to do */
    if(!_id) return;

    GLuint* bindings = Context::current()->state().buffer->bindings;

    /* Remove all current bindings from the state */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) bindings[i] = 0;

    glDeleteBuffers(1, &_id);
}

std::string Buffer::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_BUFFER, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_BUFFER_KHR, _id);
    #endif
}

Buffer& Buffer::setLabelInternal(const Containers::ArrayReference<const char> label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_BUFFER, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_BUFFER_KHR, _id, label);
    #endif
    return *this;
}

void Buffer::bind(Target target, GLuint id) {
    GLuint& bound = Context::current()->state().buffer->bindings[Implementation::BufferState::indexForTarget(target)];

    /* Already bound, nothing to do */
    if(bound == id) return;

    /* Bind the buffer otherwise */
    bound = id;
    glBindBuffer(GLenum(target), id);
}

Buffer::Target Buffer::bindInternal(Target hint) {
    GLuint* bindings = Context::current()->state().buffer->bindings;
    GLuint& hintBinding = bindings[Implementation::BufferState::indexForTarget(hint)];

    /* Shortcut - if already bound to hint, return */
    if(hintBinding == _id) return hint;

    /* Return first target in which the buffer is bound */
    /** @todo wtf there is one more? */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) return Implementation::BufferState::targetForIndex[i-1];

    /* Bind the buffer to hint target otherwise */
    hintBinding = _id;
    glBindBuffer(GLenum(hint), _id);
    return hint;
}

Int Buffer::size() {
    /**
     * @todo there is something like glGetBufferParameteri64v in 3.2 (I
     *      couldn't find any matching extension, though)
     */
    GLint size;
    (this->*Context::current()->state().buffer->getParameterImplementation)(GL_BUFFER_SIZE, &size);
    return size;
}

Buffer& Buffer::setData(const Containers::ArrayReference<const void> data, const BufferUsage usage) {
    (this->*Context::current()->state().buffer->dataImplementation)(data.size(), data, usage);
    return *this;
}

Buffer& Buffer::setSubData(const GLintptr offset, const Containers::ArrayReference<const void> data) {
    (this->*Context::current()->state().buffer->subDataImplementation)(offset, data.size(), data);
    return *this;
}

Buffer& Buffer::invalidateData() {
    (this->*Context::current()->state().buffer->invalidateImplementation)();
    return *this;
}

Buffer& Buffer::invalidateSubData(const GLintptr offset, const GLsizeiptr length) {
    (this->*Context::current()->state().buffer->invalidateSubImplementation)(offset, length);
    return *this;
}

void* Buffer::map(const MapAccess access) {
    return (this->*Context::current()->state().buffer->mapImplementation)(access);
}

#ifdef MAGNUM_TARGET_GLES2
void* Buffer::mapSub(const GLintptr offset, const GLsizeiptr length, const MapAccess access) {
    #ifdef CORRADE_TARGET_NACL
    CORRADE_ASSERT(!_mappedBuffer, "Buffer::mapSub(): the buffer is already mapped", nullptr);
    return _mappedBuffer = glMapBufferSubDataCHROMIUM(GLenum(bindInternal(_targetHint)), offset, length, GLenum(access));
    #else
    static_cast<void>(offset);
    static_cast<void>(length);
    static_cast<void>(access);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
#endif

void* Buffer::map(const GLintptr offset, const GLsizeiptr length, const MapFlags flags) {
    return (this->*Context::current()->state().buffer->mapRangeImplementation)(offset, length, flags);
}

Buffer& Buffer::flushMappedRange(const GLintptr offset, const GLsizeiptr length) {
    (this->*Context::current()->state().buffer->flushMappedRangeImplementation)(offset, length);
    return *this;
}

bool Buffer::unmap() { return (this->*Context::current()->state().buffer->unmapImplementation)(); }

#ifdef MAGNUM_TARGET_GLES2
void Buffer::unmapSub() {
    #ifdef CORRADE_TARGET_NACL
    CORRADE_ASSERT(_mappedBuffer, "Buffer::unmapSub(): the buffer is not mapped", );
    glUnmapBufferSubDataCHROMIUM(_mappedBuffer);
    _mappedBuffer = nullptr;
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::subDataInternal(GLintptr offset, GLsizeiptr size, GLvoid* data) {
    (this->*Context::current()->state().buffer->getSubDataImplementation)(offset, size, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Buffer::copyImplementationDefault(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glCopyBufferSubData(GLenum(read.bindInternal(Target::CopyRead)), GLenum(write.bindInternal(Target::CopyWrite)), readOffset, writeOffset, size);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::copyImplementationDSA(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glNamedCopyBufferSubDataEXT(read._id, write._id, readOffset, writeOffset, size);
}
#endif
#endif

void Buffer::getParameterImplementationDefault(const GLenum value, GLint* const data) {
    glGetBufferParameteriv(GLenum(bindInternal(_targetHint)), value, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::getParameterImplementationDSA(const GLenum value, GLint* const data) {
    glGetNamedBufferParameterivEXT(_id, value, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::getSubDataImplementationDefault(const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetBufferSubData(GLenum(bindInternal(_targetHint)), offset, size, data);
}

void Buffer::getSubDataImplementationDSA(const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetNamedBufferSubDataEXT(_id, offset, size, data);
}
#endif

void Buffer::dataImplementationDefault(GLsizeiptr size, const GLvoid* data, BufferUsage usage) {
    glBufferData(GLenum(bindInternal(_targetHint)), size, data, GLenum(usage));
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::dataImplementationDSA(GLsizeiptr size, const GLvoid* data, BufferUsage usage) {
    glNamedBufferDataEXT(_id, size, data, GLenum(usage));
}
#endif

void Buffer::subDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(GLenum(bindInternal(_targetHint)), offset, size, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::subDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glNamedBufferSubDataEXT(_id, offset, size, data);
}
#endif

void Buffer::invalidateImplementationNoOp() {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateImplementationARB() {
    glInvalidateBufferData(_id);
}
#endif

void Buffer::invalidateSubImplementationNoOp(GLintptr, GLsizeiptr) {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateSubImplementationARB(GLintptr offset, GLsizeiptr length) {
    glInvalidateBufferSubData(_id, offset, length);
}
#endif

void* Buffer::mapImplementationDefault(MapAccess access) {
    #ifndef MAGNUM_TARGET_GLES
    return glMapBuffer(GLenum(bindInternal(_targetHint)), GLenum(access));
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    return glMapBufferOES(GLenum(bindInternal(_targetHint)), GLenum(access));
    #else
    static_cast<void>(access);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapImplementationDSA(MapAccess access) {
    return glMapNamedBufferEXT(_id, GLenum(access));
}
#endif

void* Buffer::mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access) {
    #ifndef MAGNUM_TARGET_GLES2
    return glMapBufferRange(GLenum(bindInternal(_targetHint)), offset, length, GLenum(access));
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    return glMapBufferRangeEXT(GLenum(bindInternal(_targetHint)), offset, length, GLenum(access));
    #else
    static_cast<void>(offset);
    static_cast<void>(length);
    static_cast<void>(access);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapRangeImplementationDSA(GLintptr offset, GLsizeiptr length, MapFlags access) {
    return glMapNamedBufferRangeEXT(_id, offset, length, GLenum(access));
}
#endif

void Buffer::flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length) {
    #ifndef MAGNUM_TARGET_GLES2
    glFlushMappedBufferRange(GLenum(bindInternal(_targetHint)), offset, length);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glFlushMappedBufferRangeEXT(GLenum(bindInternal(_targetHint)), offset, length);
    #else
    static_cast<void>(offset);
    static_cast<void>(length);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::flushMappedRangeImplementationDSA(GLintptr offset, GLsizeiptr length) {
    glFlushMappedNamedBufferRangeEXT(_id, offset, length);
}
#endif

bool Buffer::unmapImplementationDefault() {
    #ifndef MAGNUM_TARGET_GLES2
    return glUnmapBuffer(GLenum(bindInternal(_targetHint)));
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    return glUnmapBufferOES(GLenum(bindInternal(_targetHint)));
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool Buffer::unmapImplementationDSA() {
    return glUnmapNamedBufferEXT(_id);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, Buffer::Target value) {
    switch(value) {
        #define _c(value) case Buffer::Target::value: return debug << "Buffer::Target::" #value;
        _c(Array)
        #ifndef MAGNUM_TARGET_GLES
        _c(AtomicCounter)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(CopyRead)
        _c(CopyWrite)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(DispatchIndirect)
        _c(DrawIndirect)
        #endif
        _c(ElementArray)
        #ifndef MAGNUM_TARGET_GLES2
        _c(PixelPack)
        _c(PixelUnpack)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(ShaderStorage)
        _c(Texture)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(TransformFeedback)
        _c(Uniform)
        #endif
        #undef _c
    }

    return debug << "Buffer::Target::(invalid)";
}
#endif

}
