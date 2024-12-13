// -------------------------------------------------------------
// 

#include <stdio.h>
#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_2, TX_SIZE_2> Can1;

// CAN command dictionary
#include "/Users/hamzadaqa/Downloads/OneShotTesting 5/MRT-CAN_ExpansionBoard-CMDdictionary.h"

const int ledPin = 13;

const int baudRate = 1000000;
const int dataFreq = 50000; // Rate at which CAN messages are sent

static CAN_message_t TXmsg;
static CAN_message_t RXmsg;

static uint8_t nodeScaler = 0;




// -------------------------------------------------------------
void setup(void)
{
  Serial.begin(115200); delay(1000);

  // Flash LED to identify Teensy
  pinMode(ledPin, OUTPUT);
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
  
  // get node number
  while(nodeScaler < 1 || nodeScaler > 4) {
    Serial.print("Enter node number (1-4): \n");
    while(!Serial.available()) {}
      nodeScaler = (uint8_t) Serial.parseInt();
  }
  Serial.print("CAN Expansion Board test: one-time event Node");
  Serial.println(nodeScaler);
  
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }

  Can1.begin();
  Can1.setBaudRate(baudRate);
  Can1.setMaxMB(16);
  //  Can1.onReceive(canSniff);
  //  Can1.mailboxStatus();

  TXmsg.len = TX_msgLength;

  Serial.println("   Ready \n");
  
}






// -------------------------------------------------------------
uint8_t CMDcount = 0;
char temp[100];

void loop(void)
{
  if ( Serial.available() ) {
    CAN_TX( Serial.parseInt() );
  }
  if ( Can1.read(RXmsg) ) {
    CAN_RX();
  }
}



// Transmit data (1 byte) as CAN message in buf[0]
void CAN_TX(int cmdCode) {
  TXmsg.buf[0] = cmdCode;

  // Adjust message ID based on command type
  if (cmdCode == 1 || cmdCode == 2) { // master commands
    TXmsg.id = M_ID;
  } else {
    TXmsg.id = nodeScaler;
  }

  // Print status of CAN TX
  sprintf(temp, "CMDcount %02u.  msgID %02lu.  buf[0] %02u.  TX Q size %02lu.  RX Q size %02lu",
          CMDcount, (unsigned long)TXmsg.id, TXmsg.buf[0],
          (unsigned long)Can1.getTXQueueCount(),
          (unsigned long)Can1.getRXQueueCount());
  Serial.println(temp);

  while (Can1.getTXQueueCount() != 0) {} // wait until TX buffer empty before sending next message

  Can1.write(TXmsg);
  Serial.println("   Message sent\n");
  
  CMDcount++;
  //    delayMicroseconds(pow(10,6)/dataFreq); // Control data rate

}



// Receive data (1 byte) as CAN message in buf[0]
void CAN_RX() {
  if( RXmsg.buf[0] == ALL_LED_OFF ||
      RXmsg.buf[0] == (ALL_LED_OFF + 10*nodeScaler) ) {
    CAN_RX_print();
    digitalWrite(ledPin, LOW);
    Serial.println("   LED off\n");
  }
  else if( RXmsg.buf[0] == ALL_LED_ON ||
           RXmsg.buf[0] == (ALL_LED_ON + 10*nodeScaler)) {
    CAN_RX_print();
    digitalWrite(ledPin, HIGH);
    Serial.println("   LED on\n");
  }
//  else {
//    Serial.println("   Command ignored\n");
//  }

  CMDcount++;
}



void CAN_RX_print() {
  sprintf(temp, "CMDcount %02u.  msgID %02lu.  buf[0] %02u.  TX Q size %02lu.  RX Q size %02lu",
          CMDcount, (unsigned long)RXmsg.id, RXmsg.buf[0],
          (unsigned long)Can1.getTXQueueCount(),
          (unsigned long)Can1.getRXQueueCount());
  Serial.println(temp);

}
