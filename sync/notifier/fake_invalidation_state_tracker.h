// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SYNC_NOTIFIER_FAKE_INVALIDATION_STATE_TRACKER_H_
#define SYNC_NOTIFIER_FAKE_INVALIDATION_STATE_TRACKER_H_

#include "base/memory/weak_ptr.h"
#include "sync/notifier/invalidation_state_tracker.h"

namespace syncer {

// InvalidationStateTracker implementation that simply keeps track of
// the max versions and invalidation state in memory.
class FakeInvalidationStateTracker
    : public InvalidationStateTracker,
      public base::SupportsWeakPtr<FakeInvalidationStateTracker> {
 public:
  FakeInvalidationStateTracker();
  virtual ~FakeInvalidationStateTracker();

  int64 GetMaxVersion(const invalidation::ObjectId& id) const;

  // InvalidationStateTracker implementation.
  virtual InvalidationStateMap GetAllInvalidationStates() const OVERRIDE;
  virtual void SetMaxVersion(const invalidation::ObjectId& id,
                             int64 max_version) OVERRIDE;
  virtual void Forget(const ObjectIdSet& ids) OVERRIDE;
  virtual void SetBootstrapData(const std::string& data) OVERRIDE;
  virtual std::string GetBootstrapData() const OVERRIDE;

  static const int64 kMinVersion;

 private:
  InvalidationStateMap state_map_;
  std::string bootstrap_data_;
};

}  // namespace syncer

#endif  // SYNC_NOTIFIER_FAKE_INVALIDATION_STATE_TRACKER_H_
