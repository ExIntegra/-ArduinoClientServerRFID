#include <Wire.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4; // число строк клавиатуры
const byte COLS = 3; // число столбцов клавиатуры

char hexaKeys[ROWS][COLS] = {
      {'1','2','3'},
      {'4','5','6'},
      {'7','8','9'},
      {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5}; // к каким выводам подключаем управление строками
byte colPins[COLS] = {4, 3, 2}; // к каким выводам подключаем управление столбцами

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup()
{
      Serial.begin(9600);
      lcd.init(); // Инициализируем экран
      lcd.backlight(); //подсветка
      lcd.setCursor(0,0); // устанавливаем курсор на начало первой строки
}

void loop()
{
  static int i = 0;
  static int attemptCounter = 1;
  static String pincodeString = "";
  char customKey = customKeypad.getKey();
  
  lcd.setCursor(0,0); // устанавливаем курсор на начало первой строки
  lcd.print("input pincode"); 

  if(customKey)
  {

    if (customKey != 0x23 && customKey != 0x2a) //0x2a = * 0x23 = #
    {
      lcd.setCursor(i, 1); // устанавливаем курсор на начало второй строки
      lcd.print(customKey);
      i++;
      pincodeString += customKey;
    }

    else if (customKey == 0x2a) //удаление символа
    {
      if (i > 0) // проверка на минимальное значение
      {
        i--;
        lcd.setCursor(i, 1); // установка курсора под следующий введенный символ
        lcd.print(" "); // заполнение строки на экране пустотой
        pincodeString.remove(pincodeString.length() - 1); // удаление из буфера последнего символа
      }
    }

    else //при отправки пинкода на сервер
    {
      if (attemptCounter == 3)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Cart is block");
        delay(2000);
        //блокируем карту на сервере и выдаем сообщение
      }

      else if(pincodeString.length() != 4) //проверка на длину пинкода
      {
        Serial.println(pincodeString.length());
        Serial.println(pincodeString);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Error input");
        delay(2000);
        i = 0;
        attemptCounter++;
        pincodeString = "";

      }

      else if(5 == 5) //если пинкод прошел
      {

        lcd.clear(); //вывод "Processing trancsaction. Wait"
        lcd.setCursor(0,0);
        lcd.print("Processing tran-");
        lcd.setCursor(0,1);
        lcd.print("saction.Wait");

        for (int j = 0; j < 4; j++) //вывод точек в режиме ожидания
        {
          lcd.print(".");
          delay(1000);
        }

        if(1 == 1) //если есть деньги
        {
          lcd.clear(); //вывод "Payment successful!"
          lcd.setCursor(4,0);
          lcd.print("Payment");
          lcd.setCursor(2,1);
          lcd.print("successful!");
          delay(2000);
          lcd.clear();
        }

        else //если денег нет
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("No money");
          delay(2000);
          lcd.clear();
          attemptCounter++;
        }

        i = 0;

      }

      else //если сервер не принял пинкода, повторяем ошибку, увеличиваем счетчик попыток на +1
      { 
        attemptCounter++;
      }
    }
  }
}
