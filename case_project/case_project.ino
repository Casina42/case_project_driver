#include <HID-Project.h>

// Definizione pin
const unsigned int PIN_FAN_CONTROL = 8;
const unsigned int PIN_FAN_BUTTON = 9;
const unsigned int PIN_VOLUME_CLK = 10;
const unsigned int PIN_VOLUME_DT = 11;
const unsigned int PIN_MUTE_BUTTON = 12;

// Conserva stato ultima posizione encoder e pulsanti
int prev_volumeClk = LOW;
int prev_volumeDt = LOW;
int stato_ventole = LOW;
int stato_muto = LOW;
int prevMuteButton = HIGH;
int prevFanButton = HIGH;

// Variabile per salvare l'utilizzo CPU ricevuto dal driver Linux
uint64_t cpu_usage = 0;

void setup() {
	// Avvia la seriale per ricevere i pacchetti a 64 bit dal driver Linux
	Serial.begin(9600);
	
	Keyboard.begin();
	Consumer.begin();

	pinMode(PIN_FAN_CONTROL, OUTPUT);
	pinMode(PIN_VOLUME_CLK, INPUT);
	pinMode(PIN_VOLUME_DT, INPUT);
	pinMode(PIN_FAN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_MUTE_BUTTON, INPUT_PULLUP);

	// Esegui prima lettura dei valori sui pin
	prev_volumeClk = digitalRead(PIN_VOLUME_CLK);
	prev_volumeDt = digitalRead(PIN_VOLUME_DT);
}

void loop() {
	// RICEZIONE PACCHETTO A 64 BIT DAL DRIVER LINUX
	if (Serial.available() >= 8) {
		// Legge esattamente 8 byte (64 bit) inviati dal modulo kernel
		Serial.readBytes((char*)&cpu_usage, 8);
	}

	int bottone_ventole = digitalRead(PIN_FAN_BUTTON);
	int volumeClk = digitalRead(PIN_VOLUME_CLK);
	int volumeDt = digitalRead(PIN_VOLUME_DT);
	int bottone_mute = digitalRead(PIN_MUTE_BUTTON);

	// Gestione Encoder Volume
	if (volumeClk != prev_volumeClk && volumeClk == HIGH) {
	if (volumeDt == volumeClk) {
		Consumer.write(MEDIA_VOLUME_DOWN);
	} else {
		Consumer.write(MEDIA_VOLUME_UP);
	}
	stato_muto = LOW;
	}
	
	// Gestione Pulsante Mute
	if (bottone_mute == LOW && prevMuteButton == HIGH) {
	stato_muto = !stato_muto;
	Consumer.write(MEDIA_VOLUME_MUTE);
	delay(20);
	}

	// Gestione Pulsante Ventole (override manuale)
	if (bottone_ventole == LOW && prevFanButton == HIGH) {
	stato_ventole = !stato_ventole;
	digitalWrite(PIN_FAN_CONTROL, stato_ventole);
	delay(20);
	}
	
	prev_volumeClk = volumeClk;
	prev_volumeDt = volumeDt;
	prevMuteButton = bottone_mute;
	prevFanButton = bottone_ventole;
}
