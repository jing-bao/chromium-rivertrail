// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_context_cgl.h"

#include <OpenGL/CGLRenderers.h>
#include <OpenGL/CGLTypes.h>
#include <vector>

#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_cgl.h"
#include "ui/gl/gpu_switching_manager.h"

namespace gfx {

GLContextCGL::GLContextCGL(GLShareGroup* share_group)
  : GLContext(share_group),
    context_(NULL),
    gpu_preference_(PreferIntegratedGpu),
    discrete_pixelformat_(NULL) {
}

bool GLContextCGL::Initialize(GLSurface* compatible_surface,
                              GpuPreference gpu_preference) {
  DCHECK(compatible_surface);

  gpu_preference = ui::GpuSwitchingManager::GetInstance()->AdjustGpuPreference(
      gpu_preference);

  GLContextCGL* share_context = share_group() ?
      static_cast<GLContextCGL*>(share_group()->GetContext()) : NULL;

  std::vector<CGLPixelFormatAttribute> attribs;
  // If the system supports dual gpus then allow offline renderers for every
  // context, so that they can all be in the same share group.
  if (ui::GpuSwitchingManager::GetInstance()->SupportsDualGpus())
    attribs.push_back(kCGLPFAAllowOfflineRenderers);
  if (GetGLImplementation() == kGLImplementationAppleGL) {
    attribs.push_back(kCGLPFARendererID);
    attribs.push_back((CGLPixelFormatAttribute) kCGLRendererGenericFloatID);
  }
  attribs.push_back((CGLPixelFormatAttribute) 0);

  CGLPixelFormatObj format;
  GLint num_pixel_formats;
  if (CGLChoosePixelFormat(&attribs.front(),
                           &format,
                           &num_pixel_formats) != kCGLNoError) {
    LOG(ERROR) << "Error choosing pixel format.";
    return false;
  }
  if (!format) {
    LOG(ERROR) << "format == 0.";
    return false;
  }
  DCHECK_NE(num_pixel_formats, 0);

  // If using the discrete gpu, create a pixel format requiring it before we
  // create the context.
  if (!ui::GpuSwitchingManager::GetInstance()->SupportsDualGpus() ||
      gpu_preference == PreferDiscreteGpu) {
    std::vector<CGLPixelFormatAttribute> discrete_attribs;
    discrete_attribs.push_back((CGLPixelFormatAttribute) 0);
    GLint num_pixel_formats;
    if (CGLChoosePixelFormat(&discrete_attribs.front(),
                             &discrete_pixelformat_,
                             &num_pixel_formats) != kCGLNoError) {
      LOG(ERROR) << "Error choosing pixel format.";
      return false;
    }
  }

  CGLError res = CGLCreateContext(
      format,
      share_context ?
          static_cast<CGLContextObj>(share_context->GetHandle()) : NULL,
      reinterpret_cast<CGLContextObj*>(&context_));
  CGLReleasePixelFormat(format);
  if (res != kCGLNoError) {
    LOG(ERROR) << "Error creating context.";
    Destroy();
    return false;
  }

  gpu_preference_ = gpu_preference;
  return true;
}

void GLContextCGL::Destroy() {
  if (discrete_pixelformat_) {
    CGLReleasePixelFormat(discrete_pixelformat_);
    discrete_pixelformat_ = NULL;
  }
  if (context_) {
    CGLDestroyContext(static_cast<CGLContextObj>(context_));
    context_ = NULL;
  }
}

bool GLContextCGL::MakeCurrent(GLSurface* surface) {
  DCHECK(context_);
  if (IsCurrent(surface))
    return true;

  TRACE_EVENT0("gpu", "GLContextCGL::MakeCurrent");

  if (CGLSetCurrentContext(
      static_cast<CGLContextObj>(context_)) != kCGLNoError) {
    LOG(ERROR) << "Unable to make gl context current.";
    return false;
  }

  SetCurrent(this, surface);
  if (!InitializeExtensionBindings()) {
    ReleaseCurrent(surface);
    return false;
  }

  if (!surface->OnMakeCurrent(this)) {
    LOG(ERROR) << "Unable to make gl context current.";
    return false;
  }

  return true;
}

void GLContextCGL::ReleaseCurrent(GLSurface* surface) {
  if (!IsCurrent(surface))
    return;

  SetCurrent(NULL, NULL);
  CGLSetCurrentContext(NULL);
}

bool GLContextCGL::IsCurrent(GLSurface* surface) {
  bool native_context_is_current = CGLGetCurrentContext() == context_;

  // If our context is current then our notion of which GLContext is
  // current must be correct. On the other hand, third-party code
  // using OpenGL might change the current context.
  DCHECK(!native_context_is_current || (GetCurrent() == this));

  if (!native_context_is_current)
    return false;

  return true;
}

void* GLContextCGL::GetHandle() {
  return context_;
}

void GLContextCGL::SetSwapInterval(int interval) {
  DCHECK(IsCurrent(NULL));
  LOG(WARNING) << "GLContex: GLContextCGL::SetSwapInterval is ignored.";
}


bool GLContextCGL::GetTotalGpuMemory(size_t* bytes) {
  DCHECK(bytes);
  *bytes = 0;

  CGLContextObj context = reinterpret_cast<CGLContextObj>(context_);
  if (!context)
    return false;

  // Retrieve the current renderer ID
  GLint current_renderer_id = 0;
  if (CGLGetParameter(context,
                      kCGLCPCurrentRendererID,
                      &current_renderer_id) != kCGLNoError)
    return false;

  // Iterate through the list of all renderers
  GLuint display_mask = static_cast<GLuint>(-1);
  CGLRendererInfoObj renderer_info = NULL;
  GLint num_renderers = 0;
  if (CGLQueryRendererInfo(display_mask,
                           &renderer_info,
                           &num_renderers) != kCGLNoError)
    return false;

  ScopedCGLRendererInfoObj scoper(renderer_info);

  for (GLint renderer_index = 0;
       renderer_index < num_renderers;
       ++renderer_index) {
    // Skip this if this renderer is not the current renderer.
    GLint renderer_id = 0;
    if (CGLDescribeRenderer(renderer_info,
                            renderer_index,
                            kCGLRPRendererID,
                            &renderer_id) != kCGLNoError)
        continue;
    if (renderer_id != current_renderer_id)
        continue;
    // Retrieve the video memory for the renderer.
    GLint video_memory = 0;
    if (CGLDescribeRenderer(renderer_info,
                            renderer_index,
                            kCGLRPVideoMemory,
                            &video_memory) != kCGLNoError)
        continue;
    *bytes = video_memory;
    return true;
  }

  return false;
}

GLContextCGL::~GLContextCGL() {
  Destroy();
}

GpuPreference GLContextCGL::GetGpuPreference() {
  return gpu_preference_;
}

void ScopedCGLDestroyRendererInfo::operator()(CGLRendererInfoObj x) const {
  CGLDestroyRendererInfo(x);
}

}  // namespace gfx
