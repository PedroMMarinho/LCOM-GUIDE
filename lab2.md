# LAB 2 Timer Driver

Ok lets start off by unzipping the content given. We should have something looking like this:

![Directory](image-4.png)

Before starting the big functions lets setup some useful small functions to help us keep a good code structer.
Looking at the ``i8254.h`` we can see some important ``macros``, that will later be used. We can start by inserting the most important macro: ``#define BIT(N) (1 << (N))``.


Next up lets look at ``utils.c``.

Lets start by setting up those functions:

<details>
  <summary>Functions Made</summary>

```c
int util_get_LSB(uint16_t val, uint8_t *lsb) {
    if(lsb == NULL) return 1;
    lsb = val;
    return 0;
}

int util_get_MSB(uint16_t val, uint8_t *msb) {
    if(msb == NULL) return 1;
    msb = val >> 8;
    return 0;
}
// Reads the value of a 8-bit register from a given port
int util_sys_inb(int port, uint8_t *value) {
    if (value == NULL) return 1;
    uint32_t val;
    int ret = sys_inb(port, &val);
    *value = val;
    return ret;
}

// Writes a value to a 8-bit register from a given port
int (util_sys_outb)(int port, uint8_t *command) {
  if (command == NULL) return 1;
  int ret = sys_outb(port, *command);
  return ret;
}
```
</details>



After thats done lets do the function ``timer_test_read_config``  and start the real deal.

Go to
[Section 3.3 of Lab2](https://pages.up.pt/~up238172/aulas/lcom2324/labs/lab2/lab2_03.html#tab2) and also check 
[Slides of class](https://pages.up.pt/~up238172/aulas/lcom2324/at/2timer.pdf).

<details>
  <summary>Functions Made</summary>

```c
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
  uint8_t st;
  if (timer_get_conf(timer, &st))
    return 1;
  
  return 1;
}

```
</details>