#include <Servo.h>

// Bartaty Inc. Dashboard for Euro Truck Simulator 2 https://github.com/Bartaty/ETS2-Dashboard-Arduino

// Назначение пинов на Arduino
const int SPEEDO_PIN      = A0; // Сигнал спидометра
const int RPM_PIN         = A1; // Сигнал тахометра
const int LEFT_INDICATOR  = 4;  // Левый поворотник
const int RIGHT_INDICATOR = 5;  // Правый поворотник
const int PARKING_BRAKE   = 6;  // Ручной тормоз
const int FUEL_WARNING    = 7;  // Заканчивается топливо
const int LOW_BEAM        = 3;  // Ближний свет
const int HIGH_BEAM       = 2;  // Дальний свет
const int MOTOR_BRAKE     = 8;  // Торможение двигателем
const int BRAKE_AIR_2     = 9;  // Ошибка давления воздуха в тормозной системе
const int BRAKE_AIR_1     = 10; // Нехватка давления воздуха в тормозной системе
const int BATTERY_LOW     = 11; // Аккумуляторная батарея разряжена
const int OIL_TEMP        = 12; // Слишком высокая температура масла (двигатель)
const int WATER_TEMP      = 13; // Слишком высокая температура воды (система охлаждения)

// Обозначение сервоприводов
Servo speedo;
Servo rpm;

#define PACKET_SYNC 0xFF
#define PACKET_VER  2
#define SERVO_DIR_NORMAL false
#define SERVO_DIR_INVERT true

int serial_byte;

void setup()
{
  Serial.begin(115200);

  // Инициализация сервоприводов
  speedo.attach(SPEEDO_PIN);
  speedo.write(180);

  rpm.attach(RPM_PIN);
  rpm.write(180);

  // Инициализация индикаторов
  pinMode(LEFT_INDICATOR, OUTPUT);
  pinMode(RIGHT_INDICATOR, OUTPUT);
  pinMode(PARKING_BRAKE, OUTPUT);
  pinMode(FUEL_WARNING, OUTPUT);
  pinMode(LOW_BEAM, OUTPUT);
  pinMode(HIGH_BEAM, OUTPUT);
  pinMode(MOTOR_BRAKE, OUTPUT);
  pinMode(BRAKE_AIR_2, OUTPUT);
  pinMode(BRAKE_AIR_1, OUTPUT);
  pinMode(BATTERY_LOW, OUTPUT);
  pinMode(OIL_TEMP, OUTPUT);
  pinMode(WATER_TEMP, OUTPUT);

  // Тестирование приборной панели
  digitalWrite(LEFT_INDICATOR, 0);
  digitalWrite(RIGHT_INDICATOR, 0);
  digitalWrite(PARKING_BRAKE, 0);
  digitalWrite(FUEL_WARNING, 0);
  digitalWrite(LOW_BEAM, 0);
  digitalWrite(HIGH_BEAM, 0);
  digitalWrite(MOTOR_BRAKE, 0);
  digitalWrite(BRAKE_AIR_2, 0);
  digitalWrite(BRAKE_AIR_1, 0);
  digitalWrite(BATTERY_LOW, 0);
  digitalWrite(OIL_TEMP, 0);
  digitalWrite(WATER_TEMP, 0);

  delay(500);

  speedo.write(0);
  rpm.write(0);
  digitalWrite(LEFT_INDICATOR, 1);
  digitalWrite(RIGHT_INDICATOR, 1);
  digitalWrite(PARKING_BRAKE, 1);
  digitalWrite(FUEL_WARNING, 1);
  digitalWrite(LOW_BEAM, 1);
  digitalWrite(HIGH_BEAM, 1);
  digitalWrite(MOTOR_BRAKE, 1);
  digitalWrite(BRAKE_AIR_2, 1);
  digitalWrite(BRAKE_AIR_1, 1);
  digitalWrite(BATTERY_LOW, 1);
  digitalWrite(OIL_TEMP, 1);
  digitalWrite(WATER_TEMP, 1);

  delay(500);

  speedo.write(180);
  rpm.write(180);
  digitalWrite(LEFT_INDICATOR, 0);
  digitalWrite(RIGHT_INDICATOR, 0);
  digitalWrite(PARKING_BRAKE, 0);
  digitalWrite(FUEL_WARNING, 0);
  digitalWrite(LOW_BEAM, 0);
  digitalWrite(HIGH_BEAM, 0);
  digitalWrite(MOTOR_BRAKE, 0);
  digitalWrite(BRAKE_AIR_2, 0);
  digitalWrite(BRAKE_AIR_1, 0);
  digitalWrite(BATTERY_LOW, 0);
  digitalWrite(OIL_TEMP, 0);
  digitalWrite(WATER_TEMP, 0);

}

void read_serial_byte_set_servo(Servo& servo, bool invert)
{
  serial_byte = Serial.read();
  serial_byte = (serial_byte < 0) ? 0 : ((serial_byte > 180) ? 180 : serial_byte);
  if (invert)
    servo.write(180 - serial_byte);
  else
    servo.write(serial_byte);
}

void skip_serial_byte()
{
  (void)Serial.read();
}

void digitalWriteFromBit(int port, int value, int shift)
{
  digitalWrite(port, (value >> shift) & 0x01);
}

void loop()
{
  if (Serial.available() < 16)
    return;

  serial_byte = Serial.read();
  if (serial_byte != PACKET_SYNC)
    return;

  serial_byte = Serial.read();
  if (serial_byte != PACKET_VER)
  {
    return;
  }

  read_serial_byte_set_servo(speedo, SERVO_DIR_INVERT); // Speed  telemetry.speed
  read_serial_byte_set_servo(rpm, SERVO_DIR_INVERT);    // RPM    telemetry.engine_rpm

  skip_serial_byte(); // Brake air pressure  telemetry.brake_air_pressure
  skip_serial_byte(); // Brake temperature   telemetry.brake_temperature
  skip_serial_byte(); // Fuel ratio          fuel_ratio
  skip_serial_byte(); // Oil pressure        telemetry.oil_pressure
  skip_serial_byte(); // Oil temperature     telemetry.oil_temperature
  skip_serial_byte(); // Water temperature   telemetry.water_temperature
  skip_serial_byte(); // Battery voltage     telemetry.battery_voltage   Ниже должен быть круиз-контроль, но его пока-что нет)

  // Байт информации о световых индикаторах грузовика, отключены незадействованные
  serial_byte = Serial.read();
//  digitalWriteFromBit(LIGHT_P,  serial_byte, 6);        // telemetry.light_parking
  digitalWriteFromBit(LEFT_INDICATOR,  serial_byte, 5); // telemetry.light_lblinker
  digitalWriteFromBit(RIGHT_INDICATOR, serial_byte, 4); // telemetry.light_rblinker
  digitalWriteFromBit(LOW_BEAM,  serial_byte, 3);       // telemetry.light_low_beam
  digitalWriteFromBit(HIGH_BEAM, serial_byte, 2);       // telemetry.light_high_beam
//  digitalWriteFromBit(LIGHT_B,  serial_byte, 1);        // telemetry.light_brake
//  digitalWriteFromBit(LIGHT_R,  serial_byte, 0);        // telemetry.light_reverse
  
  // Байт информации о индикаторах приборной панели
  serial_byte = Serial.read();  
  digitalWriteFromBit(PARKING_BRAKE, serial_byte, 7); // telemetry.parking_brake
  digitalWriteFromBit(MOTOR_BRAKE, serial_byte, 6);   // telemetry.motor_brake
  digitalWriteFromBit(BRAKE_AIR_2, serial_byte, 5);   // telemetry.brake_air_pressure_warning
  digitalWriteFromBit(BRAKE_AIR_1, serial_byte, 4);   // telemetry.brake_air_pressure_emergency
  digitalWriteFromBit(FUEL_WARNING, serial_byte, 3);  // telemetry.fuel_warning
  digitalWriteFromBit(BATTERY_LOW, serial_byte, 2);   // telemetry.battery_voltage_warning
  digitalWriteFromBit(OIL_TEMP, serial_byte, 1);      // telemetry.oil_pressure_warning
  digitalWriteFromBit(WATER_TEMP, serial_byte, 0);    // telemetry.water_temperature_warning
 
  // Enabled flags
  serial_byte = Serial.read();
  
}

