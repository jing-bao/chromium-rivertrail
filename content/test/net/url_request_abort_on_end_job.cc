// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// This class simulates what wininet does when a dns lookup fails.

#include <algorithm>
#include <cstring>

#include "base/compiler_specific.h"
#include "base/string_util.h"
#include "content/public/browser/browser_thread.h"
#include "content/test/net/url_request_abort_on_end_job.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_filter.h"
#include "net/url_request/url_request_status.h"

namespace content {
namespace {
const char kPageContent[] = "some data\r\n";
}

const char URLRequestAbortOnEndJob::k400AbortOnEndUrl[] =
    "http://url.handled.by.abort.on.end/400";

// static
void URLRequestAbortOnEndJob::AddUrlHandler() {
  net::URLRequestFilter* filter = net::URLRequestFilter::GetInstance();
  filter->AddUrlHandler(GURL(k400AbortOnEndUrl),
                        &URLRequestAbortOnEndJob::Factory);
}

// static
net::URLRequestJob* URLRequestAbortOnEndJob::Factory(
    net::URLRequest* request,
    net::NetworkDelegate* network_delegate,
    const std::string& scheme) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  return new URLRequestAbortOnEndJob(request, network_delegate);
}

// Private const version.
void URLRequestAbortOnEndJob::GetResponseInfoConst(
    net::HttpResponseInfo* info) const {
  // Send back mock headers.
  std::string raw_headers;
  if (LowerCaseEqualsASCII(k400AbortOnEndUrl,
                           request_->url().spec().c_str())) {
    raw_headers.append(
      "HTTP/1.1 400 This is not OK\n"
      "Content-type: text/plain\n");
  } else {
    NOTREACHED();
  }
  // ParseRawHeaders expects \0 to end each header line.
  ReplaceSubstringsAfterOffset(&raw_headers, 0, "\n", std::string("\0", 1));
  info->headers = new net::HttpResponseHeaders(raw_headers);
}

URLRequestAbortOnEndJob::URLRequestAbortOnEndJob(
    net::URLRequest* request, net::NetworkDelegate* network_delegate)
    : URLRequestJob(request, network_delegate),
      sent_data_(false),
      ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)) {
}

URLRequestAbortOnEndJob::~URLRequestAbortOnEndJob() {
}

void URLRequestAbortOnEndJob::GetResponseInfo(net::HttpResponseInfo* info) {
  GetResponseInfoConst(info);
}

bool URLRequestAbortOnEndJob::GetMimeType(std::string* mime_type) const {
  net::HttpResponseInfo info;
  GetResponseInfoConst(&info);
  return info.headers && info.headers->GetMimeType(mime_type);
}

void URLRequestAbortOnEndJob::StartAsync() {
  NotifyHeadersComplete();
}

void URLRequestAbortOnEndJob::Start() {
  MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&URLRequestAbortOnEndJob::StartAsync,
                 weak_factory_.GetWeakPtr()));
}

bool URLRequestAbortOnEndJob::ReadRawData(net::IOBuffer* buf,
                                          const int max_bytes,
                                          int* bytes_read) {
  if (!sent_data_) {
    *bytes_read = std::min(size_t(max_bytes), sizeof(kPageContent));
    std::memcpy(buf->data(), kPageContent, *bytes_read);
    sent_data_ = true;
    return true;
  }

  SetStatus(net::URLRequestStatus(net::URLRequestStatus::FAILED,
                                  net::ERR_CONNECTION_ABORTED));
  *bytes_read = -1;
  return false;
}

}  // namespace content
