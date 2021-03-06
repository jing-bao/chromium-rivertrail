// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/request_extra_data.h"

using WebKit::WebReferrerPolicy;
using WebKit::WebString;

namespace content {

RequestExtraData::RequestExtraData(WebReferrerPolicy referrer_policy,
                                   const WebString& custom_user_agent,
                                   bool is_main_frame,
                                   int64 frame_id,
                                   bool parent_is_main_frame,
                                   int64 parent_frame_id,
                                   bool allow_download,
                                   PageTransition transition_type,
                                   int transferred_request_child_id,
                                   int transferred_request_request_id)
    : webkit_glue::WebURLRequestExtraDataImpl(referrer_policy,
                                              custom_user_agent),
      is_main_frame_(is_main_frame),
      frame_id_(frame_id),
      parent_is_main_frame_(parent_is_main_frame),
      parent_frame_id_(parent_frame_id),
      allow_download_(allow_download),
      transition_type_(transition_type),
      transferred_request_child_id_(transferred_request_child_id),
      transferred_request_request_id_(transferred_request_request_id) {
}

RequestExtraData::~RequestExtraData() {
}

}  // namespace content
