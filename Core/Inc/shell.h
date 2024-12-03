#ifndef INC_LIB_SHELL_SHELL_H_
#define INC_LIB_SHELL_SHELL_H_

#include <stdint.h>

#define UART_DEVICE huart2
#define ARGC_MAX 8
#define BUFFER_SIZE 40
#define CMD_NAME_SIZE 20
#define SHELL_FUNC_LIST_MAX_SIZE 64

struct h_shell_struct;

typedef int (*shell_func_pointer_t)(struct h_shell_struct * h_shell, int argc, char ** argv);
typedef uint8_t (* drv_shell_transmit_t)(const char * pData, uint16_t size);
typedef uint8_t (* drv_shell_receive_t)(char * pData, uint16_t size);

typedef struct shell_func_struct {
    char *cmd_name;          // Changed from char c to char *cmd_name
    shell_func_pointer_t func;
    char *description;
} shell_func_t;

typedef struct drv_shell_struct {
    drv_shell_transmit_t transmit;
    drv_shell_receive_t receive;
} drv_shell_t;

typedef struct h_shell_struct {
    const shell_func_t *func_list;  // Changed to const pointer
    int func_list_size;
    char print_buffer[BUFFER_SIZE];
    char cmd_buffer[BUFFER_SIZE];
    drv_shell_t drv;
} h_shell_t;

void shell_init(h_shell_t * h_shell);
int shell_run(h_shell_t * h_shell);

#endif /* INC_LIB_SHELL_SHELL_H_ */
