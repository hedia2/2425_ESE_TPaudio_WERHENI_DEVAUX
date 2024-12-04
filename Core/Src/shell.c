#include "shell.h"
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "components/MCP23S17.h"




static int sh_pins(h_shell_t * h_shell, int argc, char ** argv) {


	MCP23S17_Init();
	MCP23S17_WriteRegister(MCP23S17_GPIOA, 0x00);
	return 0;
}
static int sh_status(h_shell_t * h_shell, int argc, char ** argv) {
	int size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
			"System status: OK\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return 0;
}

static int sh_led(h_shell_t * h_shell, int argc, char ** argv) {
	int size;

	if (argc != 2) {
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
				"Usage: led <0|1>\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}

	if (strcmp(argv[1], "0") == 0) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE, "LED turned OFF\r\n");
	}
	else if (strcmp(argv[1], "1") == 0) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE, "LED turned ON\r\n");
	}
	else {
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
				"Invalid argument. Use 0 or 1\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}

	h_shell->drv.transmit(h_shell->print_buffer, size);
	return 0;
}

static int sh_help(h_shell_t * h_shell, int argc, char ** argv) {
	int i;
	int size;

	if (argc > 1) {
		for (i = 0; i < h_shell->func_list_size; i++) {
			if (strcmp(h_shell->func_list[i].cmd_name, argv[1]) == 0) {
				size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
						"%s: %s\r\n", h_shell->func_list[i].cmd_name,
						h_shell->func_list[i].description);
				h_shell->drv.transmit(h_shell->print_buffer, size);
				return 0;
			}
		}
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
				"Unknown command: %s\r\n", argv[1]);
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}

	for(i = 0; i < h_shell->func_list_size; i++) {
		size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
				"%s: %s\r\n", h_shell->func_list[i].cmd_name,
				h_shell->func_list[i].description);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}

	return 0;
}

static const shell_func_t shell_functions[] = {
		{"help", sh_help, "Display help information"},
		{"status", sh_status, "Show system status"},
		{"led", sh_led, "Control LED (Usage: led <0|1>)"},
		{"allpins", sh_pins, "Control LED (Usage: led <0|1>)"},



};

void shell_init(h_shell_t * h_shell) {
	int size = 0;

	h_shell->func_list = shell_functions;
	h_shell->func_list_size = sizeof(shell_functions) / sizeof(shell_functions[0]);

	size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
			"\r\n\r\n===== Super auto radio v1.0 =====\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
			"Type 'help' for available commands\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
}

static int shell_exec(h_shell_t * h_shell, char * buf) {
	int i;
	int argc = 0;
	char *argv[ARGC_MAX];
	char *token;

	token = strtok(buf, " ");
	while (token != NULL && argc < ARGC_MAX) {
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}

	if (argc == 0) return 0;

	for (i = 0; i < h_shell->func_list_size; i++) {
		if (strcmp(h_shell->func_list[i].cmd_name, argv[0]) == 0) {
			return h_shell->func_list[i].func(h_shell, argc, argv);
		}
	}


	int size = snprintf(h_shell->print_buffer, BUFFER_SIZE,
			"Unknown command: %s\r\n", argv[0]);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return -1;
}

static const char backspace[] = "\b \b";
static const char prompt[] = "shell@radio>> ";

int shell_run(h_shell_t * h_shell) {
	int reading = 0;
	int pos = 0;

	while (1) {
		h_shell->drv.transmit(prompt, strlen(prompt));
		reading = 1;

		while(reading) {
			char c;
			h_shell->drv.receive(&c, 1);

			switch (c) {
			case '\r':
				h_shell->drv.transmit("\r\n", 2);
				h_shell->cmd_buffer[pos] = 0;
				reading = 0;
				break;

			case '\b':
				if (pos > 0) {
					pos--;
					h_shell->drv.transmit(backspace, 3);
				}
				break;

			default:
				if (pos < BUFFER_SIZE - 1) {
					h_shell->drv.transmit(&c, 1);
					h_shell->cmd_buffer[pos++] = c;
				}
			}
		}

		shell_exec(h_shell, h_shell->cmd_buffer);
		pos = 0;
	}

	return 0;
}





