// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_GLUE_SEARCHABLE_FORM_DATA_H__
#define WEBKIT_GLUE_SEARCHABLE_FORM_DATA_H__

#include <string>

#include "googleurl/src/gurl.h"

namespace WebKit {
class WebForm;
}

namespace webkit_glue {

// SearchableFormData encapsulates a URL and class name of an INPUT field
// that correspond to a searchable form request.
class SearchableFormData {
 public:
  // If form contains elements that constitutes a valid searchable form
  // request, a SearchableFormData is created and returned.
  static SearchableFormData* Create(const WebKit::WebForm& form);

  // URL for the searchable form request.
  const GURL& url() const { return url_; }

  // Encoding used to encode the form parameters; never empty.
  const std::string& encoding() const { return encoding_; }

 private:
  SearchableFormData(const GURL& url, const std::string& encoding)
      : url_(url),
        encoding_(encoding) {
  }

  const GURL url_;
  const std::string encoding_;

  DISALLOW_COPY_AND_ASSIGN(SearchableFormData);
};

}  // namespace webkit_glue

#endif // WEBKIT_GLUE_SEARCHABLE_FORM_H__
