
#ifndef SYSFS_UTILS_H
#define SYSFS_UTILS_H

int write_sysfs_attribute(const char *attr_path, const char *new_value,
			  size_t len);

#endif
