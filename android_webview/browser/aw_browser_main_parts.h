// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ANDROID_WEBVIEW_BROWSER_AW_BROWSER_MAIN_PARTS_H_
#define ANDROID_WEBVIEW_BROWSER_AW_BROWSER_MAIN_PARTS_H_

#include "content/public/browser/browser_main_parts.h"

namespace android_webview {

class AwBrowserContext;

class AwBrowserMainParts : public content::BrowserMainParts {
 public:
  AwBrowserMainParts(AwBrowserContext* browser_context);
  virtual ~AwBrowserMainParts();

  // Overriding methods from content::BrowserMainParts.
  virtual void PreEarlyInitialization() OVERRIDE;
  virtual int PreCreateThreads() OVERRIDE;
  virtual bool MainMessageLoopRun(int* result_code) OVERRIDE;

 private:
  // Android specific UI MessageLoop.
  scoped_ptr<MessageLoop> main_message_loop_;

  AwBrowserContext* browser_context_;  // weak

  DISALLOW_COPY_AND_ASSIGN(AwBrowserMainParts);
};

}  // namespace android_webview

#endif  // ANDROID_WEBVIEW_BROWSER_AW_BROWSER_MAIN_PARTS_H_
