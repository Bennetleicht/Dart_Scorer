// Copyright 2025 Hochschule Hannover
// Author : Bennet Schweer
//          Samuel Klar 
//          Chen Lu

#define cols 4
#define rows 4

// buzzer
#define buzzer_pin PD2 // D2

// lcd
#define rs PD7 // PD7 D7
#define e PB0 // PB0 D8
#define d4 PB1 // PB1 D9
#define d5 PB2 // PB2 D10
#define d6 PB3 // PB3 D11
#define d7 PB4 // PB4 D12

//keypad
#define col1 PC1 // PC1 A1
#define col2 PC0 // PC0 A0
#define col3 PD5 // PD5 D5
#define col4 PD6 // PD6 D6

#define row1 PC5 // PC5 A5
#define row2 PC4 // PC4 A4
#define row3 PC3 // PC3 A3
#define row4 PC2 // PC2 A2


char keys[cols][rows] = 
{
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

char result[4];

int value = 0;

char start_value_char[4];

int illegal_points[] = {163, 166, 169, 172, 173, 175, 178, 179};

bool game_counter = true;
int key_a_or_b_pressed = 0;

void init_buzzer()
{
  DDRD |= (1 << buzzer_pin);  // pin D2
}

void init_lcd()
{
  DDRD |= (1 << rs);  // RS output , pin D7
  DDRB |= (1 << e);  // E output , pin D8
  DDRB |= (1 << d4);  // D4 output , pin D9
  DDRB |= (1 << d5);  // D5 output , pin D10
  DDRB |= (1 << d6);  // D6 output , pin D11
  DDRB |= (1 << d7);  // D7 output , pin D12

  lcd_init();
}

void init_keyboard()
{
  // Columns as an output
  DDRD |= (1 << col4);  // pin D6
  DDRD |= (1 << col3);  // pin D5
  DDRC |= (1 << col2);  // pin A0
  DDRC |= (1 << col1);  // pin A1

  // Columns on HIGH
  PORTD |= (1 << col4);
  PORTD |= (1 << col3);
  PORTC |= (1 << col2);
  PORTC |= (1 << col1);

  // Rows as input with pull-up resistors
  DDRC &= ~(1 << row4);  // Pin A2
  DDRC &= ~(1 << row3);  // Pin A3
  DDRC &= ~(1 << row2);  // Pin A4
  DDRC &= ~(1 << row1);  // Pin A5

  //activate pull-up
  PORTC |= (1 << row4);
  PORTC |= (1 << row3);
  PORTC |= (1 << row2);
  PORTC |= (1 << row1);
}

void init_hardware()
{
  // init hardware
  if(game_counter == true)
  {
    init_buzzer();
    init_lcd();
    init_keyboard();
    game_counter = false;;
  } 
  else 
  {
    lcd_command(0x01);
    key_a_or_b_pressed = 0;
    lcd_set_cursor(0, 0);
    lcd_print("Choose game mode:");
    lcd_set_cursor(1, 1);
    lcd_print("A:501   B:301 ");
  }
}

int main(void)
{
  init_hardware();
  
  while(true) 
  {
    static int counted_inputs = 0;
    char key = scan_keypad();
    //please provide the figures we wish to transmit

    if(key == 'A' && key_a_or_b_pressed == 0) //gamemode 1
    {
        play_input_sound();
        counted_inputs = 0;
        result[0] = '\0';  // delete input
        lcd_command(0x01);
        lcd_set_cursor(0, 0);
        lcd_print("Starting value:");
        lcd_set_cursor(6, 1);
        lcd_print("501");
        value = 501;
        start_value_char[0] = '5';
        start_value_char[1] = '0';
        start_value_char[2] = '1';
        start_value_char[3] = '\0';

        _delay_ms(2000);
        key_a_or_b_pressed++;
        calculate_points(0);
    }
    if(key == 'B' && key_a_or_b_pressed == 0) //gamemode 2
    {
        play_input_sound();
        counted_inputs = 0;
        result[0] = '\0';  // delete input
        lcd_command(0x01);
        lcd_set_cursor(0, 0);
        lcd_print("Starting value:");
        lcd_set_cursor(6, 1);
        lcd_print("301");
        value = 301;
        start_value_char[0] = '3';
        start_value_char[1] = '0';
        start_value_char[2] = '1';
        start_value_char[3] = '\0';

        _delay_ms(2000);
        key_a_or_b_pressed++;
        calculate_points(0);
    }
    if(key == 'D')
    { 
      //Reset
      play_legal_point_sound();
      main();
    }
    if(key != 0 && key_a_or_b_pressed > 0) 
    {
      _delay_ms(200);  //debouncing  
     
      if(key == '#') 
      { 
        // input finished
        result[counted_inputs] = '\0';
        calculate_points(atoi(result)); //Ascii to Int 
        counted_inputs = 0;
        result[0] = '\0';
      } 
      else if(key == '*') 
      {
        // reset input
        if(counted_inputs > 0)
        {
           play_del_sound();
           counted_inputs--;
           result[counted_inputs] = '\0';
           lcd_set_cursor(9, 1);
           lcd_print("   ");  
           //clear the previously entered number and display version of it
           lcd_set_cursor(9, 1);
           lcd_print(result);
        }
      } 
      else if(counted_inputs < 3 && key != 'A' && 
              key != 'B' && key != 'C' && key != 'D') 
      {
        // store key in result list and c++
        play_input_sound();
        result[counted_inputs] = key;
        counted_inputs++;
        result[counted_inputs] = '\0';
        lcd_set_cursor(9, 1);
        lcd_print(result);
      }
    }
  }
  return 0;
}


char scan_keypad()
{
  for(int c = 0; c < cols; c++)
  {
    // set the selected column to LOW to activate it
    switch(c) 
    {
      case 0:
        PORTC &= ~(1 << col1);
        break;
      case 1:
        PORTC &= ~(1 << col2);
        break;
      case 2:
        PORTD &= ~(1 << col3);
        break;
      case 3:
        PORTD &= ~(1 << col4);
        break;
    }

    for(int r = 0; r < rows; r++) 
    {
      switch(r) 
      {
        case 0:
          if(!(PINC & (1 << row1))) // PINC reads High or Low
          {
            return keys[r][c];  // return the pressed key
          }
          break;  // row 1, A5
        case 1:
          if(!(PINC & (1 << row2))) 
          {
            return keys[r][c];  // return the pressed key
          }
          break;  // row 2, A4
        case 2:
          if(!(PINC & (1 << row3))) 
          {
            return keys[r][c];  // return the pressed key
          }
          break;  // row 3, A3
        case 3:
          if(!(PINC & (1 << row4))) 
          {
            return keys[r][c];  // return the pressed key
          }
          break;  // row 4, A2
      }
    }

    // set all columns to HIGH, deactivate the activated column
    switch(c) 
    {
      case 0:
        PORTC |= (1 << col1);
      break;  // column 1 A1
      case 1:
        PORTC |= (1 << col2);
        break;  // column 2 A0
      case 2:
        PORTD |= (1 << col3);
        break;  // column 3 D5
      case 3:
        PORTD |= (1 << col4);
        break;  // Spalte 4 D6
    }
  }
  return 0;  // no key pressed
}

////////////////////custom-written LCD library

void lcd_set_cursor(byte col, byte row)
{
  byte row_offsets[] = {0x00, 0x40};
  lcd_command(0x80 | (col + row_offsets[row]));
}

void lcd_command(byte cmd)
{
  // set RS to LOW for a command
  PORTD &= ~(1 << rs);
  send_nibble(cmd >> 4);    // sends the upper 4-bit nibble / bit shift to right 4
  send_nibble(cmd & 0x0F);  // sends the under 4-bit nibble / cmd & 0000 1111
  if(cmd == 0x01) 
  // lcd_command = delete, then delay und cursor reset, screen reset
  {
    _delay_ms(2);
    lcd_set_cursor(0, 0);
  }
}

void send_nibble(byte nibble)
{
  // decision between HIGH or LOW for a bit
  // nibble gives information about which Pin
  // controlls D4-D7
  if(nibble & 0x01) 
  {
    PORTB |= (1 << d4); // D4 High
  } 
  else 
  {
    PORTB &= ~(1 << d4);  // D4 Low
  }

  if(nibble & 0x02) 
  {
    PORTB |= (1 << d5);
  } 
  else 
  {
    PORTB &= ~(1 << d5);  
  }

  if(nibble & 0x04) 
  {
    PORTB |= (1 << d6);
  } 
  else 
  {
    PORTB &= ~(1 << d6);  
  }

  if(nibble & 0x08) 
  {
    PORTB |= (1 << d7);
  } 
  else 
  {
    PORTB &= ~(1 << d7);  
  }
  pulse_enable();
}

void pulse_enable() // enable Signal for LCD Low High Low
{
  // read and save Data from the Pins (D4-D7)
  PORTB &= ~(1 << e);  // E in LOW
  _delay_us(1);
  PORTB |= (1 << e);  // E in HIGH
  _delay_us(1);
  PORTB &= ~(1 << e);  // E in LOW
  _delay_us(100);      // delay for synchronize time
}

void lcd_print(const char* str)
{
  while(*str) 
  {
    lcd_data(*str++);
  }
}

void lcd_data(byte data)
{
  // set RS to High for data
  PORTD |= (1 << rs);
  send_nibble(data >> 4);    // send the upper 4-bit nibble
  send_nibble(data & 0x0F);  // send the under 4-bit nibble
}

void lcd_init()
{
  // general init
  _delay_ms(50);  // wait until the LCD is ready

  PORTD &= ~(1 << rs);  // RS in LOW
  PORTB &= ~(1 << e);  // E in LOW
  DDRD |= (1 << rs);    // set RS as output
  DDRB |= (1 << e);    // set E as output

  // initialisation sequence (according to HD44780 data_sheet)
  // ensures compatibility
  send_nibble(0x03);  // send the first 4 bits
  _delay_ms(5);      // wait 5ms
  send_nibble(0x03);  // again
  _delay_us(150);    // wait 150µs
  send_nibble(0x03);  // again
  _delay_us(150);    // Warte 150µs
  send_nibble(0x02);  // set 4-Bit-mode
  _delay_us(150);    // wait 150µs

  // configuration: 4-bit mode, 2 lines, 5x8 characters
  lcd_command(0x28);
  _delay_us(150);

  // Display ein, Cursor aus, Blinken aus
  lcd_command(0x0C);
  _delay_us(150);

  // clear display
  lcd_command(0x01);

  // insert mode
  lcd_command(0x06);
  _delay_us(150);
  // projekt init
  // welcome message
  lcd_set_cursor(2, 0);
  lcd_print("Dart Counter");
  lcd_set_cursor(3, 1);
  lcd_print("Good Luck!");
  _delay_ms(2500);

  // rules input
  lcd_command(0x01);
  lcd_set_cursor(0, 0);
  lcd_print("Explanation of");
  lcd_set_cursor(0, 1);
  lcd_print("the rules:");
  _delay_ms(2500);

  lcd_command(0x01);
  lcd_set_cursor(0, 0);
  lcd_print("#=Input *=Delete");
  lcd_set_cursor(0, 1);
  lcd_print("D=New Game");
  _delay_ms(4000);

  // selection of game modes on the LCD
  lcd_command(0x01);
  lcd_set_cursor(0, 0);
  lcd_print("Choose game mode:");
  lcd_set_cursor(1, 1);
  lcd_print("A:501   B:301 ");
}

void update_lcd()
{
  lcd_command(0x01);
  lcd_print("Current:");
  lcd_set_cursor(0, 1);
  lcd_print("Input:");
  lcd_set_cursor(9, 0);
  lcd_print(start_value_char);
}

////////////////////////// Logic

bool check_points(int point) 
{
  if(point > 180) 
  {
    return false;
  }
  for(int i = 0; i <= 7; i++) 
  {
    if(point == illegal_points[i]) 
    {
      return false;
    }
  }
  return true;
}

void calculate_points(int input_point) 
{
  //reset lcd
  update_lcd();

  if(!check_points(input_point)) // false insert
  {
    lcd_command(0x01);
    lcd_print("Invalid");
    lcd_set_cursor(0, 1);
    lcd_print("Input");
    play_illegal_point_sound();
    _delay_ms(500);

    update_lcd();
    return;
  }

  value -= input_point;

  if(value < 0) // Bust
  {
    lcd_command(0x01);
    lcd_set_cursor(5, 0);
    lcd_print("Bust!");
    play_illegal_point_sound();
    _delay_ms(500);
    
    value += input_point;
    update_lcd();
    return;
  } 
  else if(value == 0) // Win
  {
    lcd_command(0x01);
    lcd_print("Congrats!");
    lcd_set_cursor(0, 1);
    lcd_print("You won!");
    play_win_sound();
    _delay_ms(2000);

    lcd_command(0x01);
    lcd_print("New Game?");
    lcd_set_cursor(0, 1);
    lcd_print("Press D");
  } 
  else // normal Input
  {
    sprintf(start_value_char, "%d", value);
      if(input_point == 180)
      {
        play_one_eighty_sound();
      } 
      else 
      {
        play_legal_point_sound();
      }
    
    update_lcd();
  }
}

////////////////////////////////Sounds

void tone_on_off(int frequency, int duration_ms)
{
  for(int i = 0; i < duration_ms; i++) {
    PORTD |= (1 << buzzer_pin);             // set Pin HIGH
    delay_var(500000 / frequency);        // short stop
    PORTD &= ~(1 << buzzer_pin);            // set Pin LOW
    delay_var(500000 / frequency);        // short stop
  }
}

void play_legal_point_sound() 
{
  int notes[] = {784, 880, 988, 1047};  // G5 A5 B5 C6
  for(int i = 0; i < 4; i++) 
  {
    tone_on_off(notes[i] , 60);
    _delay_ms(70);
  }
}

void play_illegal_point_sound() 
{
  for(int i = 0; i < 3; i++) 
  {
    tone_on_off(400 , 100);
    _delay_ms(150);
  }
}

void play_win_sound() 
{
  int notes[] = {880, 988, 784, 1047, 1175};  // A5 B5 G5 C6 D6
  int durations[] = {100, 100, 80, 200, 300};

  for(int i = 0; i < 5; i++) 
  {
    tone_on_off(notes[i], durations[i]);
    delay_var(durations[i] + 20);
  }
}

void play_input_sound()
{
    tone_on_off(784, 50);
}

void play_del_sound()
{
    tone_on_off(400, 50);
}

void play_one_eighty_sound()
{
  int notes[] = {900, 800, 1200};  // A5 B5 G5 C6 D6
  int durations[] = {100, 80, 200};

  for(int i = 0; i < 3; i++) 
  {
    tone_on_off(notes[i], durations[i]);
    delay_var(durations[i] + 20);
  }
}

///////// delay for variable delays

void delay_var(unsigned int us) 
{
  while(us--) 
  {
    for(int i = 0; i < 8; i++) 
    {
      __asm__ __volatile__("nop");
    }
  }
}