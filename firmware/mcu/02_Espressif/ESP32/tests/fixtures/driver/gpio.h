/* TEST ONLY: minimal API double, never add to production includes. */
#ifndef TEST_GPIO_H
#define TEST_GPIO_H
#include <stdint.h>
typedef int gpio_num_t;
#define GPIO_NUM_MAX 40
#define GPIO_IS_VALID_GPIO(p) ((p)>=0 && (p)<40)
#define GPIO_IS_VALID_OUTPUT_GPIO(p) ((p)>=0 && (p)<34)
int gpio_set_level(gpio_num_t pin,uint32_t level);
int gpio_get_level(gpio_num_t pin);
#endif
