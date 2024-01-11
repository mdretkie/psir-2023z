/*  Demonstracyjny program "responsywny server UDP"
 *      
 *  Opis:
 *  Dziala w taki sposob, ze nasluchuje na porcie 1234 (deklaracja w makro UDP_SERVER_PORT)
 *  otrzymawszy na tym porcie wiadomosc (datagram UDP do 100B), przeksztalaca jej tresc znak po znaku 
 *  zamieniajac male litery na wielkie, a nastepnie odsyla datagram UDP do komputera 
 *  z ktorego otrzymano wiadomosc.
 * 
 *  Autor:  Aleksander Pruszkowski 
 *  Data:   2021.11.02
 */

#include <ZsutDhcp.h>           //dla pobierania IP z DHCP - proforma dla ebsim'a 
#include <ZsutEthernet.h>       //niezbedne dla klasy 'ZsutEthernetUDP'
#include <ZsutEthernetUdp.h>    //sama klasa 'ZsutEthernetUDP'
#include <ZsutFeatures.h>       //for: ZsutMillis()


#define UDP_SERVER_PORT         1234

byte MAC[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01}; //MAC adres karty sieciowej, to powinno byc unikatowe - proforma dla ebsim'a 

//dlugosc pakietu z danymi dla/z UDP
#define PACKET_BUFFER_LENGTH        120
unsigned char packetBuffer[PACKET_BUFFER_LENGTH];

//numer portu na jakim nasluchujemy 
unsigned int localPort=UDP_SERVER_PORT;    

uint16_t t;

//dla podejscia z biblioteka ZsutEthernetUdp, nie powinno sie kreowac 
//wiecej niz jednego obiektu klasy 'ZsutEthernetUDP'
ZsutEthernetUDP Udp;            

void setup() {
    //Zwyczajowe przywitanie z userem (niech wie ze system sie uruchomil poprawnie)
    Serial.begin(115200);
    Serial.print(F("Zsut eth udp server init... ["));Serial.print(F(__FILE__));
    Serial.print(F(", "));Serial.print(F(__DATE__));Serial.print(F(", "));Serial.print(F(__TIME__));Serial.println(F("]")); 

    //inicjaja karty sieciowe - proforma dla ebsim'a    
    ZsutEthernet.begin(MAC);

    //potwierdzenie na jakim IP dzialamy - proforma dla ebsim'a
    Serial.print(F("My IP address: "));
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
        Serial.print(ZsutEthernet.localIP()[thisByte], DEC);Serial.print(F("."));
    }
    Serial.println();

    //Uruchomienie nasluchiwania na datagaramy UDP
    Udp.begin(localPort);
}

void loop() {
    //czekamy na pakiet - sprawdzajac jaka jest trego dlugosc (<=0 oznacza ze nic nie otrzymalismy)
    int packetSize=Udp.parsePacket(); 
    if(packetSize>0){
        //czytamy pakiet - maksymalnie do 'PACKET_BUFFER_LENGTH' bajtow
        int len=Udp.read(packetBuffer, PACKET_BUFFER_LENGTH);
/*        if(len<=0){                     //nie ma danych - wywolujemy funkcje wymiecenia bufora
            Udp.flush();return;
        }*/

        //prezentujemy otrzymany pakiet (zakladajac ze zawiera znaki ASCII)
        Serial.print("Recieved: ");
        packetBuffer[len]='\0';
        Serial.println((char*)packetBuffer);
        
        //modyfikujemy odebrana wiadomosc - aby bylo ze cos zostalo wykonane prze serwer
        for(int j=0; j<len; j++)
            packetBuffer[j]=toupper(packetBuffer[j]);

        t=ZsutAnalog5Read();
        snprintf(&(packetBuffer[len]), PACKET_BUFFER_LENGTH-len, "(%d)", t);
        len=strlen(packetBuffer);
        
        //odsylamy pakiet do nadawcy (wywolania: Udp.remoteIP(), Udp.remotePort() - pozwola nam sie 
        //   zorentowac jaki jest jego IP i numer portu UDP)
        //UWAGA!!!  wywolania Udp.remoteIP(), Udp.remotePort() zwracaja
        //          poprawny(sensorwny) wynik jezeli jakis datagram UDP faktycznie otrzymano!!!
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(packetBuffer, len);
        Udp.endPacket();
    }
}

/*  Testy, nc to narzedzie netcat,
 *  dla windows pod https://eternallybored.org/misc/netcat/netcat-win32-1.12.zip)
 *  wywolaj z cmd (jedna linia):
 *  
 *  echo "test" | nc -w 1 -u 192.168.89.3 1234
 *  
 *  Info o nc:
 *  https://en.wikipedia.org/wiki/Netcat
 *  https://nc110.sourceforge.io/
 */
