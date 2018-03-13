/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_SPINLOCK_H
#define __ASM_SPINLOCK_H

#include <asm/qrwlock.h>
#include <asm/qspinlock.h>

/*
 * Accesses appearing in program order before a spin_lock() operation
 * can be reordered with accesses inside the critical section, by virtue
 * of arch_spin_lock being constructed using acquire semantics.
 *
 * In cases where this is problematic (e.g. try_to_wake_up), an
 * smp_mb__before_spinlock() can restore the required ordering.
 */
#define smp_mb__before_spinlock()	smp_mb()

#endif /* __ASM_SPINLOCK_H */
