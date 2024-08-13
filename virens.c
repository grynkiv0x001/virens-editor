#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disable_raw(void)
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enable_raw(void)
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");

  atexit(disable_raw);

  struct termios raw = orig_termios;

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

int main(int argc, char const *argv[])
{
  enable_raw();

  while (1) {
    char character = '\0';

  if (read(STDIN_FILENO, &character, 1) == -1 && errno != EAGAIN)
      die("read");

    if (iscntrl(character))
      printf("%d\r\n", character);
    else
      printf("%d ('%c')\r\n", character, character);

    if (character == 'q') break;
  }

  return 0;
}

