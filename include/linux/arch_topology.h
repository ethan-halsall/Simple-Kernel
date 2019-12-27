/*
 * include/linux/arch_topology.h - arch specific cpu topology information
 */
#ifndef _LINUX_ARCH_TOPOLOGY_H_
#define _LINUX_ARCH_TOPOLOGY_H_

struct sched_domain;

unsigned long arch_scale_freq_power(struct sched_domain *sd, int cpu);

unsigned long scale_cpu_capacity(struct sched_domain *sd, int cpu);

#endif /* _LINUX_ARCH_TOPOLOGY_H_ */
