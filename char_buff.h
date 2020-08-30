
#ifndef CHARBUFF
#define CHARBUFFLEN 100
// This should give a 99 char string with 1 string terminator

char char_buff[CHARBUFFLEN];
int char_buff_length = 0; // char at this position should always be 0

void char_buff_empty() {
  // Set all to zero
  for (int i=0; i<CHARBUFFLEN; i++) {
    char_buff[i] = 0;
    char_buff_length = 0;
  }
}

bool char_buff_append(String s) {
  int pos = 0;

  // While there is still space for the null terminator (-1)
  // AND characters to process
  while (char_buff_length < CHARBUFFLEN-1 && pos < s.length()) {
    // Copy the character over, and increment both pointers
    char_buff[char_buff_length] = s.charAt(pos);
    pos++;
    char_buff_length++;
  }

  // Make sure there is a null terminator
  char_buff[char_buff_length] = 0;

  return (pos==s.length());
}

bool char_buff_append(char new_buff[], int len) {
  int pos = 0;

  // While there is still space for the null terminator (-1)
  // AND characters to process
  while (char_buff_length < CHARBUFFLEN-1 && pos < len) {
    // Copy the character over, and increment both pointers
    char_buff[char_buff_length] = new_buff[pos];
    pos++;
    char_buff_length++;
  }

  // Make sure there is a null terminator
  char_buff[char_buff_length] = 0;
  
  return (pos==len);
}

void char_buff_to_string(String &str) {
  str = String(char_buff);
}

#endif
