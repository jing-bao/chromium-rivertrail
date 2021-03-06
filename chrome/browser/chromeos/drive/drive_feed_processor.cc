// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <utility>

#include "base/metrics/histogram.h"
#include "chrome/browser/chromeos/drive/document_entry_conversion.h"
#include "chrome/browser/chromeos/drive/drive_feed_processor.h"
#include "chrome/browser/chromeos/drive/drive_files.h"
#include "chrome/browser/chromeos/drive/drive_resource_metadata.h"
#include "content/public/browser/browser_thread.h"

using content::BrowserThread;

namespace drive {

class DriveFeedProcessor::FeedToEntryProtoMapUMAStats {
 public:
  FeedToEntryProtoMapUMAStats()
    : num_regular_files_(0),
      num_hosted_documents_(0) {
  }

  // Increment number of files.
  void IncrementNumFiles(bool is_hosted_document) {
    is_hosted_document ? num_hosted_documents_++ : num_regular_files_++;
  }

  // Updates UMA histograms with file counts.
  void UpdateFileCountUmaHistograms() {
    const int num_total_files = num_hosted_documents_ + num_regular_files_;
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfRegularFiles", num_regular_files_);
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfHostedDocuments",
                         num_hosted_documents_);
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfTotalFiles", num_total_files);
  }

 private:
  int num_regular_files_;
  int num_hosted_documents_;
};

DriveFeedProcessor::DriveFeedProcessor(
    DriveResourceMetadata* resource_metadata)
  : resource_metadata_(resource_metadata) {
}

DriveFeedProcessor::~DriveFeedProcessor() {
}

void DriveFeedProcessor::ApplyFeeds(
    const ScopedVector<google_apis::DocumentFeed>& feed_list,
    int64 start_changestamp,
    int64 root_feed_changestamp,
    std::set<FilePath>* changed_dirs) {
  bool is_delta_feed = start_changestamp != 0;

  resource_metadata_->set_origin(INITIALIZED);

  int64 delta_feed_changestamp = 0;
  FeedToEntryProtoMapUMAStats uma_stats;
  DriveEntryProtoMap entry_proto_map;
  FeedToEntryProtoMap(feed_list,
                      &entry_proto_map,
                      &delta_feed_changestamp,
                      &uma_stats);
  ApplyEntryProtoMap(
      entry_proto_map,
      is_delta_feed,
      is_delta_feed ? delta_feed_changestamp : root_feed_changestamp,
      changed_dirs);

  // Shouldn't record histograms when processing delta feeds.
  if (!is_delta_feed)
    uma_stats.UpdateFileCountUmaHistograms();
}

void DriveFeedProcessor::ApplyEntryProtoMap(
    const DriveEntryProtoMap& entry_proto_map,
    bool is_delta_feed,
    int64 feed_changestamp,
    std::set<FilePath>* changed_dirs) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(changed_dirs);

  if (!is_delta_feed) {  // Full update.
    resource_metadata_->root()->RemoveChildren();
    changed_dirs->insert(resource_metadata_->root()->GetFilePath());
  }
  resource_metadata_->set_largest_changestamp(feed_changestamp);

  // TODO(achuith): Get rid of this conversion of DriveEntryProtoMap to
  // ResourceMap.
  ResourceMap resource_map;
  for (DriveEntryProtoMap::const_iterator it = entry_proto_map.begin();
      it != entry_proto_map.end(); ++it) {
    scoped_ptr<DriveEntry> entry =
        resource_metadata_->CreateDriveEntryFromProto(it->second);
    resource_map.insert(std::make_pair(it->first, entry.release()));
  }

  // Go through all entries generated by the feed and apply them to the local
  // snapshot of the file system.
  for (ResourceMap::iterator it = resource_map.begin();
       it != resource_map.end();) {
    // Ensure that the entry is deleted, unless the ownership is explicitly
    // transferred by entry.release().
    scoped_ptr<DriveEntry> entry(it->second);
    DCHECK_EQ(it->first, entry->resource_id());
    // Erase the entry so the deleted entry won't be referenced.
    resource_map.erase(it++);

    DriveEntry* old_entry =
        resource_metadata_->GetEntryByResourceId(entry->resource_id());
    DriveDirectory* parent = NULL;

    if (entry->is_deleted()) {  // Deleted file/directory.
      DVLOG(1) << "Removing file " << entry->base_name();
      if (!old_entry)
        continue;

      parent = old_entry->parent();
      RemoveEntryFromParentAndCollectChangedDirectories(
          old_entry, changed_dirs);
    } else if (old_entry) {  // Change or move of existing entry.
      // Please note that entry rename is just a special case of change here
      // since name is just one of the properties that can change.
      DVLOG(1) << "Changed file " << entry->base_name();
      parent = old_entry->parent();
      if (!parent) {
        NOTREACHED();
        continue;
      }
      // Move children files over if we are dealing with directories.
      if (old_entry->AsDriveDirectory() && entry->AsDriveDirectory()) {
        entry->AsDriveDirectory()->TakeOverEntries(
            old_entry->AsDriveDirectory());
      }
      // Remove the old instance of this entry.
      RemoveEntryFromParentAndCollectChangedDirectories(
          old_entry, changed_dirs);
      // Did we actually move the new file to another directory?
      if (parent->resource_id() != entry->parent_resource_id()) {
        changed_dirs->insert(parent->GetFilePath());
        parent = FindDirectoryForNewEntry(entry.get(), resource_map);
      }
      AddEntryToDirectoryAndCollectChangedDirectories(
          entry.release(),
          parent,
          changed_dirs);
    } else {  // Adding a new file.
      parent = FindDirectoryForNewEntry(entry.get(), resource_map);
      AddEntryToDirectoryAndCollectChangedDirectories(
          entry.release(),
          parent,
          changed_dirs);
    }

    // Record the parent if it was already in the tree and this is a delta feed.
    if (parent &&
        (parent->parent() || parent == resource_metadata_->root()) &&
        is_delta_feed) {
      changed_dirs->insert(parent->GetFilePath());
    }
  }
  // All entries must be erased from the map.
  DCHECK(resource_map.empty());
}

// static
void DriveFeedProcessor::AddEntryToDirectoryAndCollectChangedDirectories(
    DriveEntry* entry,
    DriveDirectory* directory,
    std::set<FilePath>* changed_dirs) {
  if (!directory) {  // Orphan.
    delete entry;
    return;
  }
  directory->AddEntry(entry);
  if (entry->AsDriveDirectory())
    changed_dirs->insert(entry->GetFilePath());
}

// static
void DriveFeedProcessor::RemoveEntryFromParentAndCollectChangedDirectories(
    DriveEntry* entry,
    std::set<FilePath>* changed_dirs) {
  DriveDirectory* parent = entry->parent();
  if (!parent) {
    NOTREACHED();
    return;
  }

  DriveDirectory* dir = entry->AsDriveDirectory();
  if (dir) {
    // We need to notify all children of entry if entry is a directory.
    dir->GetChildDirectoryPaths(changed_dirs);
    // Notify this directory if the removed entry is a directory.
    changed_dirs->insert(dir->GetFilePath());
  }

  parent->RemoveEntry(entry);
}

DriveDirectory* DriveFeedProcessor::FindDirectoryForNewEntry(
    DriveEntry* new_entry,
    const ResourceMap& resource_map) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DriveDirectory* dir = NULL;
  // Added file.
  const std::string& parent_resource_id = new_entry->parent_resource_id();
  if (parent_resource_id.empty()) {
    dir = resource_metadata_->root();
    DVLOG(1) << "Root parent for " << new_entry->base_name();
  } else {
    DriveEntry* entry =
        resource_metadata_->GetEntryByResourceId(parent_resource_id);
    dir = entry ? entry->AsDriveDirectory() : NULL;
    if (!dir) {
      // The parent directory was also added with this set of feeds.
      ResourceMap::const_iterator iter = resource_map.find(parent_resource_id);
      if (iter != resource_map.end())
          dir = iter->second ? iter->second->AsDriveDirectory() : NULL;
    }
  }
  DVLOG(1) << new_entry->base_name() << " parent " << parent_resource_id
           << (dir ? " found" : " not found");
  return dir;
}

void DriveFeedProcessor::FeedToEntryProtoMap(
    const ScopedVector<google_apis::DocumentFeed>& feed_list,
    DriveEntryProtoMap* entry_proto_map,
    int64* feed_changestamp,
    FeedToEntryProtoMapUMAStats* uma_stats) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  for (size_t i = 0; i < feed_list.size(); ++i) {
    const google_apis::DocumentFeed* feed = feed_list[i];

    // Get upload url from the root feed. Links for all other collections will
    // be handled in ConvertDocumentEntryToDriveEntryProto.
    if (i == 0) {
      const google_apis::Link* root_feed_upload_link =
          feed->GetLinkByType(google_apis::Link::LINK_RESUMABLE_CREATE_MEDIA);
      if (root_feed_upload_link)
        resource_metadata_->root()->set_upload_url(
            root_feed_upload_link->href());
      if (feed_changestamp)
        *feed_changestamp = feed->largest_changestamp();
      DCHECK_GE(feed->largest_changestamp(), 0);
    }

    for (size_t j = 0; j < feed->entries().size(); ++j) {
      const google_apis::DocumentEntry* doc = feed->entries()[j];
      DriveEntryProto entry_proto = ConvertDocumentEntryToDriveEntryProto(*doc);
      // Some document entries don't map into files (i.e. sites).
      if (entry_proto.resource_id().empty())
        continue;

      // Count the number of files.
      if (uma_stats && entry_proto.has_file_specific_info()) {
        uma_stats->IncrementNumFiles(
            entry_proto.file_specific_info().is_hosted_document());
      }

      std::pair<DriveEntryProtoMap::iterator, bool> ret = entry_proto_map->
          insert(std::make_pair(entry_proto.resource_id(), entry_proto));
      DCHECK(ret.second);
      if (!ret.second)
        LOG(WARNING) << "Found duplicate file " << entry_proto.base_name();
    }
  }
}

}  // namespace drive
