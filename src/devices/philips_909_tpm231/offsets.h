/* Philips 65OLED909 / TPM231WW_R.201.100.146.220.
 *
 * Kernel and BTF from the exact OTA boot image (TPM231WW R.201.100.146.220).
 * All off_* values are offset from kimage_text_base = _stext before KASLR,
 * verified against /proc/kallsyms link-time addresses and BTF dump.
 * ashmem function offsets identify code bodies; the live fops->compat_ioctl
 * callback points to compat_ashmem_ioctl via a CFI jump table (CFI jumptable).
 * copy_splice_read = 0 means unavailable in this kernel.
 */
OFFSETS_ENTRY("5.15.180-android14-11",
  /* Pentonic mt5897 / merak platform. Physical base from firmware inference;
   * not observed at runtime due to SELinux Enforcing, Seccomp:0. */
  .kernel_phys_load = 0x26000000ULL,

  /* _stext is the canonical base for all off_*. */
  .kimage_text_base = 0xffffffc008010000ULL,

  /* --- Global symbol offsets from _stext --- */
  .off_init_task = 0x0298c4c0ULL,
  .off_init_cred = 0x0294e578ULL,
  .off_init_uts_ns = 0x029e2a10ULL,
  .off_empty_zero_page = 0x02a83000ULL,
  .off_root_task_group = 0x02a88400ULL,
  .off_selinux_enforcing = 0x02ada068ULL,
  .off_selinux_enforcing_device = 0x02ada068ULL,
  .off_kptr_restrict = 0x0284db84ULL,
  .off_selinux_blob_sizes = 0x01bf76f0ULL,
  .off_security_hook_heads = 0x01bf7018ULL,
  .off_kmalloc_caches = 0x01bf8278ULL,
  .off_anon_pipe_buf_ops = 0x01a356b0ULL,

  /* &ashmem_misc.fops: ashmem_misc + BTF offsetof(miscdevice,fops)=0x10. */
  .off_ashmem_misc_fops = 0x029d8f50ULL,
  .off_ashmem_fops = 0x01ba6730ULL,

  /* ashmem function offsets (code bodies; fops->compat_ioctl reaches via CFI JT). */
  .off_ashmem_ioctl = 0x0105e840ULL,
  .off_ashmem_compat_ioctl = 0x0105f244ULL,
  .off_ashmem_mmap = 0x0105f298ULL,
  .off_ashmem_open = 0x0105f548ULL,
  .off_ashmem_release = 0x0105f5e0ULL,
  .off_ashmem_show_fdinfo = 0x0105f6fcULL,

  .off_configfs_read_iter = 0x00648224ULL,
  .off_configfs_bin_write_iter = 0x00648ad4ULL,
  .off_copy_splice_read = 0,
  .off_noop_llseek = 0x00525450ULL,
  .off_cap_capable_active = 0x0085750cULL,
  .off_slide_nfulnl_logger = 0x02851ab0ULL,
  .off_slide_loggers_0_1 = 0x028519e0ULL,
  .off_slide_boot_id = 0x02b35929ULL,
  .off_init_user_ns = 0x02a05250ULL,
  .off_init_user_ns_device = 0x02a05250ULL,
  .off_system_unbound_wq = 0x02850460ULL,
  .off_call_usermodehelper_exec_work = 0x001730f8ULL,

  /* --- BTF struct field offsets (Philips mt5897 kernel) --- */

  /* task_struct fields (verified against BTF). */
  .task_usage = 0x38,
  .task_cpu = 0x58,
  .task_prio = 0x7c,
  .task_normal_prio = 0x84,
  .task_sched_task_group = 0x340,
  .task_uclamp_req = 0x348,
  .task_uclamp = 0x350,
  .task_pi_lock = 0x8ec,
  .task_pi_waiters = 0x900,
  .task_pi_top_task = 0x910,
  .task_pi_blocked_on = 0x918,
  .task_pid = 0x640,
  .task_tgid = 0x644,
  .task_real_parent = 0x650,
  .task_atomic_flags = 0x600,
  .task_real_cred = 0x7f8,
  .task_cred = 0x800,
  .task_comm = 0x810,
  .task_tasks = 0x538,
  .task_seccomp = 0x8c8,

  /* rt_mutex_waiter (BTF type 7842, size=0x58). */
  .waiter_compact = 1,
  .waiter_size = 0x58,
  .waiter_pi_tree = 0x18,
  .waiter_task = 0x30,
  .waiter_lock = 0x38,
  .waiter_wake_state = 0x40,
  .waiter_prio = 0x44,
  .waiter_deadline = 0x48,
  .waiter_ww_ctx = 0x50,

  /* rt_mutex_base (BTF type 7844, size=0x20). */
  .rt_mutex_size = 0x20,
  .rt_mutex_waiters = 0x08,
  .rt_mutex_owner = 0x18,

  /* cred (BTF type 612, size=0xb0). */
  .cred_usage = 0x00,
  .cred_uid = 0x04,
  .cred_gid = 0x08,
  .cred_suid = 0x0c,
  .cred_sgid = 0x10,
  .cred_euid = 0x14,
  .cred_egid = 0x18,
  .cred_fsuid = 0x1c,
  .cred_fsgid = 0x20,
  .cred_securebits = 0x24,
  .cred_cap_inh = 0x28,
  .cred_cap_prm = 0x30,
  .cred_cap_eff = 0x38,
  .cred_cap_bset = 0x40,
  .cred_cap_amb = 0x48,
  .cred_security = 0x78,
  .cred_user = 0x80,
  .cred_user_ns = 0x88,
  .cred_ucounts = 0x90,
  .cred_group_info = 0x98,

  /* struct page: mm_struct_size=0x3e0 -> slab order 2 (size=1024). */
  .mm_struct_size = 0x3e0,
  .struct_page_size = 0x40,

  /* fops table layout. */
  .fops_llseek = 0x08,
  .fops_read = 0x10,
  .fops_write = 0x18,
  .fops_read_iter = 0x20,
  .fops_write_iter = 0x28,
  .fops_ioctl = 0x50,
  .fops_compat_ioctl = 0x58,
  .fops_mmap = 0x60,
  .fops_open = 0x70,
  .fops_release = 0x80,
  .fops_splice_read = 0xc8,
  .fops_show_fdinfo = 0xe0,
)
