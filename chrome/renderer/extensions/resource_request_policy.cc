// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/extensions/resource_request_policy.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/url_constants.h"
#include "chrome/common/extensions/extension.h"
#include "chrome/common/extensions/extension_set.h"
#include "content/public/common/page_transition_types.h"
#include "googleurl/src/gurl.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebConsoleMessage.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"

namespace extensions {

// static
bool ResourceRequestPolicy::CanRequestResource(
    const GURL& resource_url,
    WebKit::WebFrame* frame,
    content::PageTransition transition_type,
    const ExtensionSet* loaded_extensions) {
  CHECK(resource_url.SchemeIs(chrome::kExtensionScheme));

  const Extension* extension =
      loaded_extensions->GetExtensionOrAppByURL(ExtensionURLInfo(resource_url));
  if (!extension) {
    // Allow the load in the case of a non-existent extension. We'll just get a
    // 404 from the browser process.
    return true;
  }

  // Disallow loading of packaged resources for hosted apps. We don't allow
  // hybrid hosted/packaged apps. The one exception is access to icons, since
  // some extensions want to be able to do things like create their own
  // launchers.
  std::string resource_root_relative_path =
      resource_url.path().empty() ? "" : resource_url.path().substr(1);
  if (extension->is_hosted_app() &&
      !extension->icons().ContainsPath(resource_root_relative_path)) {
    LOG(ERROR) << "Denying load of " << resource_url.spec() << " from "
               << "hosted app.";
    return false;
  }

  // Disallow loading of extension resources which are not explicitely listed
  // as web accessible if the manifest version is 2 or greater.
  if (!extension->IsResourceWebAccessible(resource_url.path()) &&
      !CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableExtensionsResourceWhitelist)) {
    GURL frame_url = frame->document().url();
    GURL page_url = frame->top()->document().url();

    // Exceptions are:
    // - empty origin (needed for some edge cases when we have empty origins)
    bool is_empty_origin = frame_url.is_empty();
    // - extensions requesting their own resources (frame_url check is for
    //     images, page_url check is for iframes)
    bool is_own_resource = frame_url.GetOrigin() == extension->url() ||
        page_url.GetOrigin() == extension->url();
    // - devtools (chrome-extension:// URLs are loaded into frames of devtools
    //     to support the devtools extension APIs)
    bool is_dev_tools = page_url.SchemeIs(chrome::kChromeDevToolsScheme) &&
        !extension->devtools_url().is_empty();
    bool transition_allowed =
        !content::PageTransitionIsWebTriggerable(transition_type);
    // - unreachable web page error page (to allow showing the icon of the
    //   unreachable app on this page)
    bool is_error_page = frame_url == GURL(content::kUnreachableWebDataURL);

    if (!is_empty_origin && !is_own_resource &&
        !is_dev_tools && !transition_allowed && !is_error_page) {
      std::string message = base::StringPrintf(
          "Denying load of %s. Resources must be listed in the "
          "web_accessible_resources manifest key in order to be loaded by "
          "pages outside the extension.",
          resource_url.spec().c_str());
      frame->addMessageToConsole(
          WebKit::WebConsoleMessage(WebKit::WebConsoleMessage::LevelError,
                                    WebKit::WebString::fromUTF8(message)));
      return false;
    }
  }

  return true;
}

// static
bool ResourceRequestPolicy::CanRequestExtensionResourceScheme(
    const GURL& resource_url,
    WebKit::WebFrame* frame) {
  CHECK(resource_url.SchemeIs(chrome::kExtensionResourceScheme));

  GURL frame_url = frame->document().url();
  if (!frame_url.is_empty() &&
      !frame_url.SchemeIs(chrome::kExtensionScheme)) {
    std::string message = base::StringPrintf(
        "Denying load of %s. chrome-extension-resources:// can only be "
        "loaded from extensions.",
      resource_url.spec().c_str());
    frame->addMessageToConsole(
        WebKit::WebConsoleMessage(WebKit::WebConsoleMessage::LevelError,
                                  WebKit::WebString::fromUTF8(message)));
    return false;
  }

  return true;
}

ResourceRequestPolicy::ResourceRequestPolicy() {
}

}  // namespace extensions
