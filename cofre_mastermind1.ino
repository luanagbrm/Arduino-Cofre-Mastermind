#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>
#include <stdlib.h>
#include <time.h>

Servo servo;

const byte LINHAS = 4;
const byte COLUNAS = 4;

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte PINOS_LINHAS[LINHAS] = {9, 8, 7, 6};
byte PINOS_COLUNAS[COLUNAS] = {5, 4, 3, 2};

Keypad teclado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS);

LiquidCrystal_I2C lcd(0x27,16,2);

int codigoSecreto[4];
int digitado[4];
int j;

int luzVermelha = 11;
int luzAmarela = 10;
int luzVerde = 13;

void gerarCodigo() {
    for (int i = 0; i < 4; i++) {
        int novoDig;
        int unico;

        do {
            unico = 1;
            novoDig = 1 + rand() % 8; // 

            for (int j = 0; j < i; j++) {
                if (codigoSecreto[j] == novoDig) {
                    unico = 0;
                    break;
                }
            }
        } while (!unico);

        codigoSecreto[i] = novoDig;
    }
}

void desligarTodas() {
  digitalWrite(luzVermelha, LOW);
  digitalWrite(luzAmarela, LOW);
  digitalWrite(luzVerde, LOW);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  servo.attach(12);
  servo.write(0);
  srand( (unsigned)time(0) );
  pinMode(luzVermelha, OUTPUT);
  pinMode(luzAmarela, OUTPUT);
  pinMode(luzVerde, OUTPUT);
}

void loop() {

  char tecla = teclado.getKey();
  
  //Liga o cofre e inicia o jogo
  if (tecla == 'A') {
    servo.write(90);
    gerarCodigo();
    lcd.setBacklight(HIGH);
    delay(1000);
    lcd.clear();
    
    // Loop principal do jogo
    while (true) {
      // Loop para capturar a entrada do usuário
      while (true) {
        tecla = teclado.getKey();
        
  
        if (tecla != NO_KEY && tecla != 'A') {
          for (j = 0; j < 4; j++) {
            if (digitado[j] == 0) {
              digitado[j] = tecla - '0';
              lcd.print(tecla);
              break;
            }
          }
        }
        
        //Apaga tudo o que foi digitado
        if(tecla == 'D'){
          lcd.clear();
          for (int i = 0; i < 4; i++) {
             digitado[i] = 0;
          }
        }
        
        
        //Envia o código digitado para validação
        if (tecla == 'C') {
          // Verifica se a senha está correta
          int certo = 0, meio = 0;
          for (int i = 0; i < 4; i++) {
            if (digitado[i] == codigoSecreto[i]) {
              certo++;
            } else {
              for (int j = 0; j < 4; j++) {
                if (digitado[i] == codigoSecreto[j]) {
                  meio++;
                  break;
                }
              }
            }
          }
          lcd.clear();
          
          if (certo == 4) {
            digitalWrite(luzVerde, HIGH);
            lcd.print("PARABENS!");
            lcd.setCursor(0, 1);
            lcd.print("Voce acertou!");
            servo.write(180);
            delay(5000);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("'B' para jogar");
            lcd.setCursor(0, 1);
            lcd.print("'*' para sair");
            
            // Loop para aguardar a escolha do usuário
            while (true) {
              tecla = teclado.getKey();
              
              // Se 'B' for pressionado, reinicia o jogo
              if (tecla == 'B') {
                desligarTodas();
                gerarCodigo();
                lcd.clear();
                // Reseta as variáveis
                for (int i = 0; i < 4; i++) {
                  digitado[i] = 0;
                }
                servo.write(90);
                break; // Sai do loop interno
              } else if (tecla == '*') {
                lcd.clear();
                lcd.setBacklight(LOW);
                for (int i = 0; i < 4; i++) {
                  digitado[i] = 0;
                }
                return;
              }
            }
            break;
            
          } else {
            if(meio == 0 && certo == 0)
              digitalWrite(luzVermelha, HIGH);
            else
              digitalWrite(luzAmarela, HIGH);
            lcd.print("digitado: ");
            
            for (int i = 0; i < 4; i++) {
              lcd.print(digitado[i]);
            }

            lcd.setCursor(0,1);
            lcd.print("C: ");
            lcd.print(certo);
            lcd.print(" |L: ");
            lcd.print(meio);
            lcd.print(" |E: ");
            lcd.print(4 - (certo + meio));
            
            delay(5000);
            lcd.clear();
            desligarTodas();

            for (int i = 0; i < 4; i++) {
              digitado[i] = 0;
            }
          }
        }
      }
    }
  }
}
