#ifndef Magnum_DefaultFramebuffer_h
#define Magnum_DefaultFramebuffer_h
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

/** @file
 * @brief Class @ref Magnum::DefaultFramebuffer
 */

#include "Magnum/AbstractFramebuffer.h"

namespace Magnum {

/**
@brief Default framebuffer

Default framebuffer, i.e. the actual screen surface. It is automatically
created when @ref Context is created and it is available through global
variable @ref defaultFramebuffer. It is by default mapped to whole screen
surface.

@anchor DefaultFramebuffer-usage
## Usage

When you are using only the default framebuffer, the usage is simple. You
must ensure that it is properly resized when application surface is resized,
i.e. you must pass the new size in your @ref Platform::Sdl2Application::viewportEvent() "viewportEvent()"
implementation, for example:
@code
void viewportEvent(const Vector2i& size) {
    defaultFramebuffer.setViewport({}, size);

    // ...
}
@endcode

Next thing you probably want is to clear all used buffers before performing
any drawing in your @ref Platform::Sdl2Application::drawEvent() "drawEvent()"
implementation, for example:
@code
void drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth);

    // ...
}
@endcode

See Framebuffer documentation for more involved usage, usage of non-default or
multiple framebuffers.

## Performance optimizations

See also @ref AbstractFramebuffer-performance-optimization "relevant section in AbstractFramebuffer".

If on desktop GL and either @extension{ARB,direct_state_access} (part of OpenGL
4.5) or @extension{EXT,direct_state_access} is available, functions
@ref checkStatus(), @ref mapForDraw(), @ref mapForRead() and @ref invalidate()
use DSA to avoid unnecessary calls to @fn_gl{BindFramebuffer}. See their
respective documentation for more information.
*/
class MAGNUM_EXPORT DefaultFramebuffer: public AbstractFramebuffer {
    friend Context;

    public:
        /**
         * @brief Status
         *
         * @see @ref checkStatus()
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        enum class Status: GLenum {
            /** The framebuffer is complete */
            Complete = GL_FRAMEBUFFER_COMPLETE,

            /**
             * The default framebuffer does not exist.
             * @requires_gles30 Extension @es_extension{OES,surfaceless_context}
             *      in OpenGL ES 2.0
             */
            #ifndef MAGNUM_TARGET_GLES2
            Undefined = GL_FRAMEBUFFER_UNDEFINED
            #else
            Undefined = GL_FRAMEBUFFER_UNDEFINED_OES
            #endif
        };

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw attachment
         *
         * @see @ref mapForDraw()
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        enum class DrawAttachment: GLenum {
            /** Don't use the output. */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Write output to front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Write output to front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Write output to back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Write output to back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,
            #endif

            /**
             * Write output to back buffer.
             *
             * On desktop OpenGL, this is equal to
             * @ref DrawAttachment::BackLeft.
             */
            #ifdef MAGNUM_TARGET_GLES
            Back = GL_BACK,
            #else
            Back = GL_BACK_LEFT,
            #endif

            /**
             * Write output to front buffer.
             *
             * On desktop OpenGL, this is equal to
             * @ref DrawAttachment::FrontLeft.
             */
            #ifdef MAGNUM_TARGET_GLES
            Front = GL_FRONT
            #else
            Front = GL_FRONT_LEFT
            #endif
        };
        #endif

        /**
         * @brief Read attachment
         *
         * @see @ref mapForRead()
         * @requires_gles30 Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         *      in OpenGL ES 2.0
         */
        enum class ReadAttachment: GLenum {
            /** Don't read from any buffer */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Read from front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Read from front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Read from back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Read from back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Read from left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Left = GL_LEFT,

            /**
             * Read from right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Right = GL_RIGHT,
            #endif

            /** Read from back buffer. */
            Back = GL_BACK,

            /**
             * Read from front buffer.
             * @requires_es_extension Extension @es_extension2{NV,read_buffer_front,GL_NV_read_buffer}
             */
            Front = GL_FRONT

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Read from front and back buffer.
             * @requires_gl In OpenGL ES you must specify either
             *      @ref Magnum::DefaultFramebuffer::ReadAttachment "ReadAttachment::Front"
             *      or @ref Magnum::DefaultFramebuffer::ReadAttachment "ReadAttachment::Back".
             */
            FrontAndBack = GL_FRONT_AND_BACK
            #endif
        };

        /**
         * @brief Invalidation attachment
         *
         * @see @ref invalidate()
         * @requires_gl43 Extension @extension{ARB,invalidate_subdata}
         * @requires_gles30 Extension @es_extension{EXT,discard_framebuffer}
         *      in OpenGL ES 2.0
         */
        enum class InvalidationAttachment: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Invalidate front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Invalidate front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Invalidate back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Invalidate back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,
            #endif

            /** Invalidate color buffer. */
            #ifndef MAGNUM_TARGET_GLES2
            Color = GL_COLOR,
            #else
            Color = GL_COLOR_EXT,
            #endif

            /** Invalidate depth bufer. */
            #ifndef MAGNUM_TARGET_GLES2
            Depth = GL_DEPTH,
            #else
            Depth = GL_DEPTH_EXT,
            #endif

            /** Invalidate stencil buffer. */
            #ifndef MAGNUM_TARGET_GLES2
            Stencil = GL_STENCIL
            #else
            Stencil = GL_STENCIL_EXT
            #endif
        };

        explicit MAGNUM_LOCAL DefaultFramebuffer();

        /** @brief Copying is not allowed */
        DefaultFramebuffer(const DefaultFramebuffer&) = delete;

        /** @brief Moving is not allowed */
        DefaultFramebuffer(DefaultFramebuffer&&) = delete;

        /** @brief Copying is not allowed */
        DefaultFramebuffer& operator=(const DefaultFramebuffer&) = delete;

        /** @brief Moving is not allowed */
        DefaultFramebuffer& operator=(DefaultFramebuffer&& other) = delete;

        /**
         * @brief Check framebuffer status
         * @param target    Target for which to check the status
         *
         * If on OpenGL ES or neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} is available,
         * the framebuffer is bound before the operation (if not already).
         * @see @fn_gl2{CheckNamedFramebufferStatus,CheckFramebufferStatus},
         *      @fn_gl_extension{CheckNamedFramebufferStatus,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{CheckFramebufferStatus}
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        Status checkStatus(FramebufferTarget target);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Map shader outputs to buffer attachment
         * @return Reference to self (for method chaining)
         *
         * @p attachments is list of shader outputs mapped to buffer
         * attachments. Shader outputs which are not listed are not used, you
         * can achieve the same by passing @ref DrawAttachment::None as
         * attachment. Example usage:
         * @code
         * defaultFramebuffer.mapForDraw({{MyShader::ColorOutput, DefaultFramebuffer::DrawAttachment::BackLeft},
         *                                {MyShader::NormalOutput, DefaultFramebuffer::DrawAttachment::None}});
         * @endcode
         *
         * If on OpenGL ES or neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} is available,
         * the framebuffer is bound before the operation (if not already).
         * @see @ref maxDrawBuffers(), @ref maxDualSourceDrawBuffers(),
         *      @ref mapForRead(), @fn_gl2{NamedFramebufferDrawBuffers,DrawBuffers},
         *      @fn_gl_extension{FramebufferDrawBuffers,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{DrawBuffers}
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        DefaultFramebuffer& mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments);

        /**
         * @brief Map shader output to buffer attachment
         * @param attachment        Buffer attachment
         * @return Reference to self (for method chaining)
         *
         * Similar to above function, can be used in cases when shader has
         * only one (unnamed) output.
         *
         * If on OpenGL ES or neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} is available,
         * the framebuffer is bound before the operation (if not already).
         * @see @ref mapForRead(), @fn_gl2{NamedFramebufferDrawBuffer,DrawBuffer},
         *      @fn_gl_extension{FramebufferDrawBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{DrawBuffer} or
         *      @fn_gl{DrawBuffers} in OpenGL ES 3.0
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        DefaultFramebuffer& mapForDraw(DrawAttachment attachment);
        #endif

        /**
         * @brief Map given attachment for reading
         * @param attachment        Buffer attachment
         * @return Reference to self (for method chaining)
         *
         * If on OpenGL ES or neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} is available,
         * the framebuffer is bound before the operation (if not already).
         * @see @ref mapForDraw(), @fn_gl2{NamedFramebufferReadBuffer,ReadBuffer},
         *      @fn_gl_extension{FramebufferReadBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{ReadBuffer}
         * @requires_gles30 Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         *      in OpenGL ES 2.0
         */
        DefaultFramebuffer& mapForRead(ReadAttachment attachment);

        /**
         * @brief Invalidate framebuffer
         * @param attachments       Attachments to invalidate
         *
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3), extension @es_extension{EXT,discard_framebuffer} in OpenGL ES
         * 2.0 or OpenGL ES 3.0 is not available, this function does nothing.
         * If @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @fn_gl2{InvalidateNamedFramebufferData,InvalidateFramebuffer},
         *      eventually @fn_gl{InvalidateFramebuffer} or
         *      @fn_gles_extension{DiscardFramebuffer,EXT,discard_framebuffer}
         *      on OpenGL ES 2.0
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Invalidate framebuffer rectangle
         * @param attachments       Attachments to invalidate
         * @param rectangle         Rectangle to invalidate
         *
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3) is not available, this function does nothing. If
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref invalidate(std::initializer_list<InvalidationAttachment>),
         *      @fn_gl2{InvalidateNamedFramebufferSubData,InvalidateSubFramebuffer},
         *      eventually @fn_gl{InvalidateSubFramebuffer}
         * @requires_gles30 Use @ref Magnum::DefaultFramebuffer::invalidate(std::initializer_list<InvalidationAttachment>) "invalidate(std::initializer_list<InvalidationAttachment>)"
         *      in OpenGL ES 2.0 instead.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle);
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        DefaultFramebuffer& setViewport(const Range2Di& rectangle) {
            AbstractFramebuffer::setViewport(rectangle);
            return *this;
        }
        DefaultFramebuffer& clear(FramebufferClearMask mask) {
            AbstractFramebuffer::clear(mask);
            return *this;
        }
        #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);
};

/** @brief Default framebuffer instance */
extern DefaultFramebuffer MAGNUM_EXPORT defaultFramebuffer;

/** @debugoperatorclassenum{Magnum::DefaultFramebuffer,Magnum::DefaultFramebuffer::Status} */
Debug MAGNUM_EXPORT operator<<(Debug debug, DefaultFramebuffer::Status value);

}

#endif
