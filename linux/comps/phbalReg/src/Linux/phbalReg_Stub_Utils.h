/*
 * phbalReg_Stub_Utils.h
 *
 *  Created on: 26 Jan 2016
 *      Author: donatien
 */

#ifndef PHBALREG_STUB_UTILS_H_
#define PHBALREG_STUB_UTILS_H_

#include <stddef.h>
#include <stdbool.h>

int gpio_is_exported(size_t gpio);
int gpio_export(size_t gpio);
int gpio_unexport(size_t gpio);
int gpio_set_direction(size_t gpio, bool output);
int gpio_set_edge(size_t gpio, bool rising, bool falling);
int gpio_open(size_t gpio, int mode);

#endif /* PHBALREG_STUB_UTILS_H_ */
