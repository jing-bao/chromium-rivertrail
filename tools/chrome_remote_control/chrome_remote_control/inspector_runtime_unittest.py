# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
from chrome_remote_control import inspector_runtime
from chrome_remote_control import tab_test_case

class InspectorRuntimeTest(tab_test_case.TabTestCase):
  def testRuntimeEvaluateSimple(self):
    res = self._tab.runtime.Evaluate('1+1')
    assert res == 2

  def testRuntimeEvaluateThatFails(self):
    self.assertRaises(inspector_runtime.EvaluateException,
                      lambda: self._tab.runtime.Evaluate('fsdfsdfsf'))

  def testRuntimeEvaluateOfSomethingThatCantJSONize(self):

    def test():
      self._tab.runtime.Evaluate("""
        var cur = {};
        var root = {next: cur};
        for (var i = 0; i < 1000; i++) {
          next = {};
          cur.next = next;
          cur = next;
        }
        root;""")
    self.assertRaises(inspector_runtime.EvaluateException, test)

  def testRuntimeExecuteOfSomethingThatCantJSONize(self):
    self._tab.runtime.Execute('window')
