// File contains general services expected from tm.
#ifndef SB7_TM_SPEC_H_
#define SB7_TM_SPEC_H_

#include <stdint.h>
#include "../common/tid.h"

namespace sb7 {
	void global_init_tm(int number_threads);

	void thread_init_tm();

	void global_clean_tm();

	void thread_clean_tm();

	void *tm_read_word(void *addr);

	void tm_write_word(void *addr, void *val);

    static void* get_thread();

    static void* get_thread(unsigned id);

    extern void* thread_desc[];
}

inline void sb7::global_init_tm(int number_threads) {
}

inline void sb7::thread_init_tm() {
}

inline void sb7::global_clean_tm() {
}

inline void sb7::thread_clean_tm() {
}

inline void* sb7::tm_read_word(void* addr) {
    return *(intptr_t*)addr;
}

inline void sb7::tm_write_word(void* addr, void* val) {
    *(intptr_t*)addr = (intptr_t)val;
}

static inline void* sb7::get_thread() {
    return get_thread(get_tid());
}

static inline void* sb7::get_thread(unsigned id) {
    return thread_desc[id];
}


// in addition to defining these functions, tm specific functions
// must also define macros: TX_DATA, TX_START, TX_COMMIT and TX_ABORT

// now include tm specific files


#endif