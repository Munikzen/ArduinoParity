#define MSG_TEXT "test"
#define PARITY_TYPE "E"
#define TX 1

static int error_counter = 0;
static int tx_counter = 0;
static String msg(MSG_TEXT);
static bool ACK = false;
static String parity_mode(PARITY_TYPE);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(50);
  Serial2.begin(115200);
  Serial2.setTimeout(50);
}

void loop() {
    if (tx_counter < 1000)
    {
      if (TX)
      {
        serial_tx(msg);
        tx_counter++;
        Serial.println("Number of transmissions: " + String(tx_counter));
      }
      else
        serial_rx(msg);
    }
    else
      while(1);
}

void serial_tx(String& msg)
{
  int length = strlen(parity_mode);
  if (parity_mode[length - 1] == 'E')
    bool curr_parity = 1;
  else
    bool curr_parity = 0;
  auto message_w_parity = add_parity(msg, curr_parity);
  message_w_parity = message_w_parity + parity_mode;
  Serial1.print(message_w_parity);
  Serial1.flush();
  Serial.println("Sending message: " + msg);
  while(Serial1.available() == 0);
  Serial.println(char(Serial1.read()));
}

void serial_rx(String& msg)
{
  Serial.println("Waiting for a message");
  while(Serial2.available() == 0);
  auto msg_rcv = Serial2.readString();

  int length = strlen(msg_rcv);
  if (msg_rcv[length - 1] == 'E'){      // ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    bool curr_parity = 1;
    msg_rcv[length-1] = '\0';
  }
  else{
    bool curr_parity = 0;
    msg_rcv[length-1] = '\0';
  }
  auto msg_decoded = decode_message(msg_rcv, curr_parity);
  // if (msg != msg_rcv)
  //  error_counter++;
  Serial.println(msg_decoded);
  tx_counter++;
  Serial.println("Number of errors: " + String(error_counter) + ". Number of received messages: " + String(tx_counter));
  Serial2.write("A"); //K
  Serial2.flush();
}

String add_parity(const &String message, bool parity){ //parity true = Even, false = Odd
  for (int i = 0; i < message.length() - 1; i++){
    int bit_count = 0;
    for(int j = 0; j < 8; j++)
      bit_count += bitRead(message[i], j); 
    if(parity && (bit_count % 2) != 0)
      message[i] ^= 128;
    else if(parity = false && (bit_count % 2) == 0)
      message[i] ^= 128;
  }
  return message;
}

String decode_message(const &String message, bool parity){ //parity true = Even, false = Odd
  for (int i = 0; i < message.length() - 1; i++){
    int bit_count = 0;
    for(int j = 0; j < 8; j++)
      bit_count += bitRead(message[i], j); 
    if(parity && (bit_count % 2) == 0) //if parity is odd and character is even
      error_counter++; //increment the error counter
    else if(parity == false && (bit_count % 2) != 0) //if parity is even and character is odd
      error_counter++; //increment the error counter
    if(bitRead(message[i], 7) == 1) //if the MSB is 1
      message[i] ^= 128; //flip the MSB to restore the original character
  }
  return message; //return the decodified message
}

// corromper el bit 33
// i = noBit / LongChar = 33/8 = 4
// se debe corromper el 4to caracter

// noBit % 8 = 1
// 1 << 1 (NumDespl) = 2 (posicionDelBit)

// bit_error = random(str.length);
// i = bi_error / 8;
// str[i] XOR 1 << (bit_error % 8);

// bit de paridad
// ej @ = 64 0100 0000
// 1) Contar el numero de unos
// 2) Invertir el bit mas significativo del caracter según la paridad
// str[i] XOR = 128;

// se debe programar las 2 paridades      

// CRC (successs / numIteraciones)