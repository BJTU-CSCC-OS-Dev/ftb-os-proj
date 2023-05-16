#define sync_test_and_set(p, v) __sync_lock_test_and_set((p), (v))
#define sync_reset(p) __sync_lock_release((p))
#define dead_loop() while(1) {}
#define loop(c) while((c)) {}
#define wall() __sync_synchronize()