/* Host-only profile validation. No exploit code or device access. */
#include <stdio.h>
#include <string.h>
#include "offsets.h"

static int require(int condition, const char *message) {
  if (condition) return 0;
  fprintf(stderr, "FAIL: %s\n", message);
  return 1;
}

int main(void) {
  const char *release = "5.15.180-android14-11-00042-g766efc243647-dirty";
  const struct kernel_offsets *profile = NULL;
  int matches = 0, failed = 0;
  for (unsigned i = 0; known_offsets[i].uname_r; ++i) {
    if (strcmp(known_offsets[i].uname_r, release) == 0) {
      profile = &known_offsets[i];
      ++matches;
    }
  }
  failed |= require(matches == 1, "Philips build must match exactly once");
  if (!profile) return 1;
  failed |= require(profile->analysis_only == 1, "Philips must remain analysis-only");
  failed |= require(ghost_profile_requires_analysis(profile), "profile must refuse activation");
  struct kernel_offsets changed = *profile;
  changed.analysis_only = 0;
  failed |= require(ghost_profile_requires_analysis(&changed),
                    "clearing analysis_only must not activate unverified routes");
  failed |= require(profile->kimage_text_base + profile->off_init_task ==
                    UINT64_C(0xffffffc00a99c4c0), "init_task address mismatch");
  failed |= require(profile->kimage_text_base + profile->off_init_cred ==
                    UINT64_C(0xffffffc00a95e578), "init_cred address mismatch");
  failed |= require(profile->kimage_text_base == UINT64_C(0xffffffc008000000),
                    "physical conversions require the _text basis");
  failed |= require(profile->phys_offset == UINT64_C(0x20000000),
                    "foreign physical offset must not be inherited");
  failed |= require(profile->user_ns_size == 0x290 && profile->user_ns_ucounts == 0x208 &&
                    profile->uid_gid_map_extent == 8, "supporting layouts must match Philips BTF");
  failed |= require(profile->payload_fake_user + profile->user_struct_size <=
                    profile->payload_fake_ucounts, "user and ucounts slots overlap");
  failed |= require(profile->direct_map_end == 0 && profile->mm_slab_order == 0,
                    "unknown runtime geometry must not be guessed");
  if (!failed) puts("PASS: Philips profile is consistent and remains blocked");
  return failed;
}
