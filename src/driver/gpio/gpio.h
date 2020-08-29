#pragma once

typedef enum {
  gpioa,
  gpiob,
  gpioc,
  gpiod,
  gpioe,
  gpiof,
} gpiox_t;

typedef enum {
  gpio_pin0,
  gpio_pin1,
  gpio_pin2,
  gpio_pin3,
  gpio_pin4,
  gpio_pin5,
  gpio_pin6,
  gpio_pin7,
  gpio_pin8,
  gpio_pin9,
  gpio_pin10,
  gpio_pin11,
  gpio_pin12,
  gpio_pin13,
  gpio_pin14,
  gpio_pin15,
} gpio_pin_t;

typedef enum {
  gpio_analog,
  gpio_float,
  gpio_pull_down,
  gpio_pull_up,
} gpio_input_cnf_t;

typedef enum {
  gpio_gen_opp,  // General purpose output push-pull
  gpio_gen_opd,  // General purpose output Open-drain
  gpio_af_opp,   // Alternate function output Push-pull
  gpio_af_opd,   // Alternate function output Open-drain
} gpio_output_cnf_t;

typedef enum {
  gpio_input,
  gpio_output_10M,
  gpio_output_2M,
  gpio_output_50M,
} gpio_io_mode_t;

typedef const struct {
  /*GPIO config*/
  gpiox_t port;
  gpio_pin_t pinx;
  gpio_io_mode_t io_mode;  // gpio_input etc.
  uint8_t cnf;             // gpio_input_cnf_t or gpio_output_cnf_t

  /*AFIO config*/
  uint32_t af_remap;  // 0 for disable
  uint32_t af_remap_mask;
} gpio_cfg_t;

extern void init_gpio(gpio_cfg_t *gpio_cfg);

extern void reset_pin(gpiox_t port, gpio_pin_t pinx);
extern void set_pin(gpiox_t port, gpio_pin_t pinx);
extern int get_pin(gpiox_t port, gpio_pin_t pinx);
