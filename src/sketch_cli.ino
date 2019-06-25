#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#define LINE_BUF_SIZE 128   //Maximum input string length
#define ARG_BUF_SIZE 64     //Maximum argument string length
#define MAX_NUM_ARGS 8      //Maximum number of arguments

int LEDpin = 13;
int blink_cycles = 10;      //How many times the LED will blink
bool error_flag = false;

char line[LINE_BUF_SIZE];
char args[MAX_NUM_ARGS][ARG_BUF_SIZE];

//Function declarations
int cmd_help();
int cmd_led();
int cmd_gpio();
int cmd_exit();

//List of functions pointers corresponding to each command
int (*commands_func[])() {
  &cmd_help,
  &cmd_led,
  &cmd_gpio,
  &cmd_exit
};

//List of command names
const char *commands_str[] = {
  "help",
  "led",
  "gpio",
  "exit"
};

//List of LED sub command names
const char *led_args[] = {
  "on",
  "off",
  "blink"
};

//List of GPIO pin
const int GPIO_PIN[] = {
  22,  23,  24,  25,
  26,  27,  28,  29,
  30,  31,  32,  33,
  34,  35,  36,  37
};

int num_commands = sizeof(commands_str) / sizeof(char *);

void setup() {
  Serial.begin(115200);

  init_led();
  init_gpio();

  cli_init();
}

void init_led() {
  // Initialize led
  pinMode(LEDpin, OUTPUT);
  }

void init_gpio() {
  // Initialize gpio
  for (int i = 0; i < sizeof(GPIO_PIN) / sizeof(GPIO_PIN[0]); i++) {
    pinMode(GPIO_PIN[i], OUTPUT);
  }
}

void loop() {
  my_cli();
}

void cli_init() {
  Serial.println("Welcome to this simple Arduino command line interface (CLI).");
  Serial.println("Type \"help\" to see a list of commands.");
}


void my_cli() {
  Serial.print("> ");

  read_line();
  if (!error_flag) {
    parse_line();
  }
  if (!error_flag) {
    execute();
  }

  memset(line, 0, LINE_BUF_SIZE);
  memset(args, 0, sizeof(args[0][0]) * MAX_NUM_ARGS * ARG_BUF_SIZE);

  error_flag = false;
}

void read_line() {
  String line_string;

  while (!Serial.available());

  if (Serial.available()) {
    line_string = Serial.readStringUntil('\n');
    if (line_string.length() < LINE_BUF_SIZE) {
      line_string.toCharArray(line, LINE_BUF_SIZE);
      Serial.println(line_string);
    }
    else {
      Serial.println("Input string too long.");
      error_flag = true;
    }
  }
}

void parse_line() {
  char *argument;
  int counter = 0;

  argument = strtok(line, " ");

  while ((argument != NULL)) {
    if (counter < MAX_NUM_ARGS) {
      if (strlen(argument) < ARG_BUF_SIZE) {
        strcpy(args[counter], argument);
        argument = strtok(NULL, " ");
        counter++;
      }
      else {
        Serial.println("Input string too long.");
        error_flag = true;
        break;
      }
    }
    else {
      break;
    }
  }
}

int execute() {
  for (int i = 0; i < num_commands; i++) {
    if (strcmp(args[0], commands_str[i]) == 0) {
      return (*commands_func[i])();
    }
  }

  Serial.println("Invalid command. Type \"help\" for more.");
  return 0;
}

int cmd_help() {
  if (args[1] == NULL) {
    help_help();
  }
  else if (strcmp(args[1], commands_str[0]) == 0) {
    help_help();
  }
  else if (strcmp(args[1], commands_str[1]) == 0) {
    help_led();
  }
  else if (strcmp(args[1], commands_str[2]) == 0) {
    help_gpio();
  }
  else if (strcmp(args[1], commands_str[3]) == 0) {
    help_exit();
  }
  else {
    help_help();
  }
}

void help_help() {
  Serial.println("The following commands are available:");

  for (int i = 0; i < num_commands; i++) {
    Serial.print("  ");
    Serial.println(commands_str[i]);
  }
  Serial.println("");
  Serial.println("You can for instance type \"help led\" for more info on the LED command.");
}

void help_led() {
  Serial.print("Control the on-board LED, either on, off or blinking ");
  Serial.print(blink_cycles);
  Serial.println(" times:");
  Serial.println("  led on");
  Serial.println("  led off");
  Serial.println("  led blink hz");
  Serial.println("    where \"hz\" is the blink frequency in Hz.");
}

void help_exit() {
  Serial.println("This will exit the CLI. To restart the CLI, restart the program.");
}

int cmd_led() {
  if (strcmp(args[1], led_args[0]) == 0) {
    Serial.println("Turning on the LED.");
    digitalWrite(LEDpin, HIGH);
  }
  else if (strcmp(args[1], led_args[1]) == 0) {
    Serial.println("Turning off the LED.");
    digitalWrite(LEDpin, LOW);
  }
  else if (strcmp(args[1], led_args[2]) == 0) {
    if (atoi(args[2]) > 0) {
      Serial.print("Blinking the LED ");
      Serial.print(blink_cycles);
      Serial.print(" times at ");
      Serial.print(args[2]);
      Serial.println(" Hz.");

      int delay_ms = (int)round(1000.0 / atoi(args[2]) / 2);

      for (int i = 0; i < blink_cycles; i++) {
        digitalWrite(LEDpin, HIGH);
        delay(delay_ms);
        digitalWrite(LEDpin, LOW);
        delay(delay_ms);
      }
    }
    else {
      Serial.println("Invalid frequency.");
    }
  }
  else {
    Serial.println("Invalid command. Type \"help led\" to see how to use the LED command.");
  }
}

int cmd_exit() {
  Serial.println("Exiting CLI.");

  while (1);
}

void help_gpio() {
  Serial.println("Read or write the gpio. Valid GPIO number ranges from 22 to 37.");
  Serial.println("To read gpio No.22:");
  Serial.println("  gpio 22");
  Serial.println("To write 1 to gpio No.37:");
  Serial.println("  gpio 37 1");
}

//int cmd_gpio() {
//  return 0;
//}

int cmd_gpio() {

  char *endptr;
  errno = 0;

  /* Convert the provided value to a decimal long */
  long int gpio_pin = strtol(args[1], &endptr, 10);
  if (endptr == args[1])
  {
    // nothing parsed from the string, handle errors or exit
    Serial.print("Conversion error occurred: ");
    Serial.println(errno);
    return 1;
  }
  //if ((gpio_pin == LONG_MAX || gpio_pin == LONG_MIN) && errno == ERANGE)
  if ((gpio_pin>37 || gpio_pin<22) || errno == ERANGE)
  {
    // out of range, handle or exit
    Serial.println("The value provided was out of range.");
    return 1;
  }

  if (strcmp(args[2], "0") == 0) {
    digitalWrite(gpio_pin, 0);
  } else if (strcmp(args[2], "1") == 0) {
    digitalWrite(gpio_pin, 1);
  } else if (strcmp(args[2], "") == 0) {
    int val = digitalRead(gpio_pin);
    Serial.println(val);
  } else {
    Serial.println("Invalid command.");
  }
}
