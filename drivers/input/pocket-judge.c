/*
 * Copyright 2017 Paranoid Android
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/string.h>

#include "touchscreen/synaptics_driver_s3320_custom.h"
#include "fingerprint/fpc/fpc1020_tee_custom.h"

/**
 * This driver maintains a sysfs interface used by the pocket bridge system
 * service. It enables and disables interrupts based on pocket state to
 * optimize battery consumption in-pocket.
 *
 * @author Chris Lahaye
 * @hide
 */

static bool pocket_judge_inpocket = false;
EXPORT_SYMBOL(pocket_judge_inpocket);

static void pocket_judge_update(void)
{
	synaptics_s3320_enable_global(!pocket_judge_inpocket);
	fpc1020_enable_global(!pocket_judge_inpocket);
}

static ssize_t inpocket_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	return sprintf(buf, "%u\n", pocket_judge_inpocket);
}

static ssize_t inpocket_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t size)
{
	bool state;
	ssize_t ret;

	ret = strtobool(buf, &state);
	if (ret)
		return size;

	if (pocket_judge_inpocket != state) {
		pocket_judge_inpocket = state;
		pocket_judge_update();
	}

	return size;
}

static DEVICE_ATTR_RW(inpocket);

static struct attribute *pocket_judge_class_attrs[] = {
	&dev_attr_inpocket.attr,
	NULL,
};

static const struct attribute_group pocket_judge_attr_group = {
	.attrs = pocket_judge_class_attrs,
};

static struct kobject *pocket_judge_kobj;

static int __init pocket_judge_init(void)
{
	ssize_t ret;

	pocket_judge_kobj = kobject_create_and_add("pocket_judge", kernel_kobj);
	if (!pocket_judge_kobj)
		return -ENOMEM;

	ret = sysfs_create_group(pocket_judge_kobj, &pocket_judge_attr_group);
	if (ret)
		kobject_put(pocket_judge_kobj);

	return ret;
}

static void __exit pocket_judge_exit (void)
{
	kobject_put(pocket_judge_kobj);
}

module_init(pocket_judge_init);
module_exit(pocket_judge_exit);
