/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sector_storage/stores/impl/local_store.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "primitives/sector_file/sector_file.hpp"

namespace fc::sector_storage::stores {

  LocalStore::LocalStore(std::shared_ptr<LocalStorage> storage,
                         std::shared_ptr<SectorIndex> index,
                         gsl::span<std::string> urls)
      : index_(index), urls_(urls.begin(), urls.end()) {}

  outcome::result<AcquireSectorResponse> LocalStore::acquireSector(
      SectorId sector,
      RegisteredProof seal_proof_type,
      SectorFileType existing,
      SectorFileType allocate,
      bool can_seal) {
    if ((existing | allocate) != (existing ^ allocate)) {
      return outcome::success();  // TODO: ERROR
    }

    // mutex_.lock_shared();

    AcquireSectorResponse result{};
    result.release = []() {
      // unlock mutex
    };

    for (const auto &type : primitives::sector_file::kSectorFileTypes) {
      if ((type & existing) == 0) {
        continue;
      }

      auto sectors_info_opt = index_->storageFindSector(sector, type, false);

      if (sectors_info_opt.has_error()) {
        // TODO: logger warning
        continue;
      }

      for (const auto &info : sectors_info_opt.value()) {
        auto store_path_iter = paths_.find(info.id);
        if (store_path_iter == paths_.end()) {
          continue;
        }

        if (store_path_iter->second.empty()) {
          continue;
        }

        boost::filesystem::path spath(store_path_iter->second);
        spath /= toString(type);
        spath /= primitives::sector_file::sectorName(sector);

        result.paths.setPathByType(type, spath.string());
        result.stores.setPathByType(type, info.id);

        existing = static_cast<SectorFileType>(existing ^ type);
        break;
      }
    }

    for (const auto &type : primitives::sector_file::kSectorFileTypes) {
      if ((type & allocate) == 0) {
        continue;
      }

      auto sectors_info_opt =
          index_->storageBestAlloc(type, seal_proof_type, can_seal);

      if (sectors_info_opt.has_error()) {
        // TODO: unlock mutex
        // TODO: logger
        return sectors_info_opt.error();
      }

      std::string best_path;
      StorageID best_storage;

      auto req_size_opt =
          fc::primitives::sector_file::sealSpaceUse(type, seal_proof_type);

      if (req_size_opt.has_error()) {
        // TODO: Unlock mutex
        return req_size_opt.error();
      }
      for (const auto &info : sectors_info_opt.value()) {
        auto path_iter = paths_.find(info.id);
        if (path_iter == paths_.end()) {
          continue;
        }

        if (path_iter->second.empty()) {
          continue;
        }

        if (can_seal && !info.can_seal) {
          continue;
        }

        if (!can_seal && !info.can_store) {
          continue;
        }

        boost::filesystem::path spath(path_iter->second);
        spath /= toString(type);
        spath /= primitives::sector_file::sectorName(sector);

        best_path = spath.string();
        best_storage = info.id;
      }

      if (best_path.empty()) {
        // TODO: Unlock mutex
        return outcome::success();  // TODO: ERROR
      }

      result.paths.setPathByType(type, best_path);
      result.stores.setPathByType(type, best_storage);
      allocate = static_cast<SectorFileType>(allocate ^ type);
    }

    return result;
  }

  outcome::result<void> LocalStore::remove(SectorId sector,
                                           SectorFileType types,
                                           bool force) {
    return outcome::success();
  }

  outcome::result<void> LocalStore::moveStorage(SectorId sector,
                                                RegisteredProof seal_proof_type,
                                                SectorFileType types) {
    return outcome::success();
  }

  outcome::result<fc::primitives::FsStat> LocalStore::getFsStat(
      fc::primitives::StorageID id) {
    return outcome::success();
  }
}  // namespace fc::sector_storage::stores