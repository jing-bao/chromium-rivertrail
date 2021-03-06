// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/resource/resource_bundle.h"

#import <UIKit/UIKit.h>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/mac/bundle_locations.h"
#include "base/mac/foundation_util.h"
#include "base/memory/ref_counted_memory.h"
#include "base/memory/scoped_nsobject.h"
#include "base/synchronization/lock.h"
#include "base/sys_string_conversions.h"
#include "ui/base/resource/resource_handle.h"
#include "ui/gfx/image/image.h"

namespace ui {

namespace {

FilePath GetResourcesPakFilePath(NSString* name, NSString* mac_locale) {
  NSString *resource_path;
  if ([mac_locale length]) {
    resource_path = [base::mac::FrameworkBundle() pathForResource:name
                                                           ofType:@"pak"
                                                      inDirectory:@""
                                                  forLocalization:mac_locale];
  } else {
    resource_path = [base::mac::FrameworkBundle() pathForResource:name
                                                           ofType:@"pak"];
  }
  if (!resource_path) {
    // Return just the name of the pak file.
    return FilePath(base::SysNSStringToUTF8(name) + ".pak");
  }
  return FilePath([resource_path fileSystemRepresentation]);
}

}  // namespace

void ResourceBundle::LoadCommonResources() {
  AddDataPackFromPath(GetResourcesPakFilePath(@"chrome", nil),
                      ui::SCALE_FACTOR_100P);
  AddDataPackFromPath(GetResourcesPakFilePath(@"resources", nil),
                      ui::SCALE_FACTOR_100P);
  AddDataPackFromPath(
      GetResourcesPakFilePath(@"theme_resources_100_percent", nil),
      SCALE_FACTOR_100P);
  AddDataPackFromPath(GetResourcesPakFilePath(@"ui_resources_100_percent", nil),
                      SCALE_FACTOR_100P);
  // TODO(rohitrao): This is where Mac loads 2x images on Lion; we should do the
  // same if needed. http://crbug.com/154291.
}

FilePath ResourceBundle::GetLocaleFilePath(const std::string& app_locale,
                                           bool test_file_exists) {
  NSString* mac_locale = base::SysUTF8ToNSString(app_locale);

  // iOS uses "_" instead of "-", so swap to get a iOS-style value.
  mac_locale = [mac_locale stringByReplacingOccurrencesOfString:@"-"
                                                     withString:@"_"];

  // On disk, the "en_US" resources are just "en" (http://crbug.com/25578).
  if ([mac_locale isEqual:@"en_US"])
    mac_locale = @"en";

  FilePath locale_file_path = GetResourcesPakFilePath(@"locale", mac_locale);

  if (delegate_) {
    locale_file_path =
        delegate_->GetPathForLocalePack(locale_file_path, app_locale);
  }

  // Don't try to load empty values or values that are not absolute paths.
  if (locale_file_path.empty() || !locale_file_path.IsAbsolute())
    return FilePath();

  if (test_file_exists && !file_util::PathExists(locale_file_path))
    return FilePath();

  return locale_file_path;
}

gfx::Image& ResourceBundle::GetNativeImageNamed(int resource_id, ImageRTL rtl) {
  // Flipped images are not used on iOS.
  DCHECK_EQ(rtl, RTL_DISABLED);

  // Check to see if the image is already in the cache.
  {
    base::AutoLock lock(*images_and_fonts_lock_);
    ImageMap::iterator found = images_.find(resource_id);
    if (found != images_.end()) {
      return found->second;
    }
  }

  gfx::Image image;
  if (delegate_)
    image = delegate_->GetNativeImageNamed(resource_id, rtl);

  if (image.IsEmpty()) {
    // Load the raw data from the resource pack.
    // TODO(rohitrao): Load 200P resources as needed.  http://crbug.com/154291.
    scoped_refptr<base::RefCountedStaticMemory> data(
        LoadDataResourceBytes(resource_id, ui::SCALE_FACTOR_100P));

    // Create a data object from the raw bytes.
    scoped_nsobject<NSData> ns_data(
        [[NSData alloc] initWithBytes:data->front() length:data->size()]);

    // Create the image from the data. The gfx::Image will take ownership.
    scoped_nsobject<UIImage> ui_image([[UIImage alloc] initWithData:ns_data]);

    if (!ui_image.get()) {
      LOG(WARNING) << "Unable to load image with id " << resource_id;
      NOTREACHED();  // Want to assert in debug mode.
      return GetEmptyImage();
    }

    image = gfx::Image(ui_image.release());
  }

  base::AutoLock lock(*images_and_fonts_lock_);

  // Another thread raced the load and has already cached the image.
  if (images_.count(resource_id))
    return images_[resource_id];

  images_[resource_id] = image;
  return images_[resource_id];
}

}  // namespace ui
