#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>

//minified web page
const char page[] PROGMEM  = {"<!DOCTYPE html> <html> <head> <title>Arduino Web Page</title> <script>function GetSwitchAnalogData(){nocache=\"?nocache=\" + Math.random() * 1000000; var request=new XMLHttpRequest(); request.onreadystatechange=function(){if (this.readyState==4){if (this.status==200){if (this.responseText !=null){document.getElementById(\"sw_an_data\").innerHTML=this.responseText;}}}}request.open(\"GET\", \"ajax_switch.php\" + nocache, true); request.send(null); setTimeout('GetSwitchAnalogData()', 1000);}</script> </head> <body onload=\"GetSwitchAnalogData()\"> <h1>Arduino AJAX Input</h1> <div id=\"sw_an_data\"> </div></body> </html>"};

// MAC address from Ethernet shield sticker under board
//Please set this to the device's actual MAC or a unique value if more than one instance will be on your network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 20); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String HTTP_req;            // stores the HTTP request
unsigned int ADCValue;
double Voltage;
double Vcc;

int k;  //counter for space savvy print from progmem
char myChar; //buffer for space savvy print from progmem

void setup()
{
    // Attempt DHCP configuration
    if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // initialize the Ethernet device not using DHCP:
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    }
    

    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(7, INPUT);        // switch is attached to Arduino pin 7
    pinMode(8, INPUT);        // switch is attached to Arduino pin 8
    Serial.write(12);
    Serial.print("Server is up!");
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();

                    
                    // SERVE request for /ajax_switch.php when requested
                    if (HTTP_req.indexOf("ajax_switch.php") > -1) {
                        // read switch state and analog input
                        //named .php only for ease of testing web logic on a development server
                        GetAjaxData(client);
                    }
                    
                    // SERVE request for /settings/ when requested
                    if (HTTP_req.indexOf("settings") > -1) {
                        //content of "settings" page to replace body div here
                        // this is to be another state but has not yet been implemented
                    }

                    
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls, stored in PROGMEM

                        int len = strlen_P(page);
                        for (k = 0; k < len; k++)
                          {
                              myChar =  pgm_read_byte_near(page + k);
                              client.print(myChar);
                          }
                        
                        
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// send the state of the switch to the web browser
void GetAjaxData(EthernetClient cl)
{
    
    if (digitalRead(7)) {
        cl.println("<p>Switch 7 state: ON</p>");
    }
    else {
        cl.println("<p>Switch 7 state: OFF</p>");
    }
    if (digitalRead(8)) {
        cl.println("<p>Switch 8 state: ON</p>");
    }
    else {
        cl.println("<p>Switch 8 state: OFF</p>");
    }
    // read analog pin A2
    Vcc = readVcc()/1000.0;
    ADCValue = analogRead(2);
    Voltage = (ADCValue / 1024.0) * Vcc;
    cl.print("<p>Analog A2: ");
    cl.print(Voltage);
    cl.println("</p>");
}

//read the voltage value of our voltage reference for SUPER ACCURATE analog reads!
//this could be done once perhaps and the constant added to the formula above to save space
//who knows what's going on between the USB regulator to the vref however, 
//better to run every time in a development envronment and enforce lean coding :D
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}
