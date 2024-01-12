#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include "cffi_rw.h"

int serial_port;

int setup_serial(char *port) {
  // Ref:
  // https://en.wikibooks.org/wiki/Serial_Programming/termios#Opening/Closing_a_Serial_Device
  // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#baud-rate

  serial_port = open(port, O_RDWR);

  // Check for errors
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
    return serial_port;
  }

  // Create new termios struct, we call it 'tty' for convention
  // No need for "= {0}" at the end as we'll immediately write the existing
  // config to this struct
  struct termios tty;

  // Read in existing settings, and handle any error
  // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
  // must have been initialized with a call to tcgetattr() overwise behaviour
  // is undefined
  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  }

  // tty.c_oflag = 0;
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

  tty.c_cflag &= ~(CSIZE | PARENB);
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS;

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  if (cfsetspeed(&tty, B460800) < 0) {
    printf("Error in trying to set baud rate");
  }

  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }

  return serial_port;
}

int close_serial(void) {
  if (serial_port >= 0) {
    close(serial_port);
    serial_port = 0;
  }
  return serial_port;
}

int my_rhd_rw_serial(uint16_t *tx_buf, uint16_t *rx_buf, size_t len) {
  // my_rhd_rw(tx_buf, rx_buf, len);
  return write(serial_port, tx_buf, len * 2);
}

int my_rhd_rw(uint16_t *tx_buf, uint16_t *rx_buf, size_t len) {
  printf("R/W %d words to SPI: ", (int)len);
  for (int i = 0; i < len; i++) {
    printf("0x%x ", tx_buf[i]);
  }
  printf("\n");
  return 1;
}