// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_OS_BLUESTORE_STUPIDALLOCATOR_H
#define CEPH_OS_BLUESTORE_STUPIDALLOCATOR_H

#include <mutex>

#include "Allocator.h"
#include "include/btree_interval_set.h"
#include "os/bluestore/bluestore_types.h"

class StupidAllocator : public Allocator {
  CephContext* cct;
  std::mutex lock;

  int64_t num_free;     ///< total bytes in freelist
  int64_t num_reserved; ///< reserved bytes

  std::vector<btree_interval_set<uint64_t> > free;        ///< leading-edge copy

  uint64_t last_alloc;

  unsigned _choose_bin(uint64_t len);
  void _insert_free(uint64_t offset, uint64_t len);

public:
  StupidAllocator(CephContext* cct);
  ~StupidAllocator();

  int reserve(uint64_t need);
  void unreserve(uint64_t unused);

  int allocate(
    uint64_t want_size, uint64_t alloc_unit, uint64_t max_alloc_size,
    int64_t hint, mempool::bluestore_alloc::vector<AllocExtent> *extents, int *count, uint64_t *ret_len);

  int allocate_int(
    uint64_t want_size, uint64_t alloc_unit, int64_t hint,
    uint64_t *offset, uint32_t *length);

  int release(
    uint64_t offset, uint64_t length);

  uint64_t get_free();

  void dump() override;

  void init_add_free(uint64_t offset, uint64_t length);
  void init_rm_free(uint64_t offset, uint64_t length);

  void shutdown();
};

#endif
