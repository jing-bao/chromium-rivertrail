// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "chrome/browser/ui/webui/options/options_ui_browsertest.h"
#include "chrome/common/chrome_switches.h"

namespace {

// Same as OptionsBrowserTest but launches with Guest mode command line
// switches.
class GuestModeOptionsBrowserTest : public options::OptionsBrowserTest {
 public:
   GuestModeOptionsBrowserTest() : OptionsBrowserTest() {}

   virtual void SetUpCommandLine(CommandLine* command_line) OVERRIDE {
    command_line->AppendSwitch(switches::kGuestSession);
    command_line->AppendSwitch(switches::kIncognito);
  }
};

IN_PROC_BROWSER_TEST_F(GuestModeOptionsBrowserTest, FLAKY_LoadOptionsByURL) {
  NavigateToSettings();
  VerifyTitle();
  VerifyNavbar();
}

}  // namespace
