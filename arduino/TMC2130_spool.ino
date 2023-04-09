/**
 * spool feeder with TMC2130 SPI
 * PIN 4 = limit switch or other sensor input
*/
#define EN_PIN           7  // Enable
#define DIR_PIN          5  // Direction
#define STEP_PIN         6  // Step
#define CS_PIN           10 // Chip select
#define SW_MOSI          11 // Software Master Out Slave In (MOSI)
#define SW_MISO          12 // Software Master In Slave Out (MISO)
#define SW_SCK           13 // Software Slave Clock (SCK)

int val = 0;
int cc = 0;
constexpr uint32_t steps_per_mm = 80;

#include <TMC2130Stepper.h>
TMC2130Stepper driver = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

void setup() {
    SPI.begin();
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Start...");
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    pinMode(4, INPUT);
    
    driver.begin();             // Initiate pins and registeries
    driver.rms_current(600);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    driver.stealthChop(1);      // Enable extremely quiet stepping
    driver.stealth_autoscale(1);
    driver.microsteps(16);

    
    stepper.setMaxSpeed(4000);
    stepper.setAcceleration(2000);
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(true, false, true);
    stepper.enableOutputs();
}

void loop() {
    val = digitalRead(4);
    if (val == HIGH){
       stepper.move(50*steps_per_mm); // Move 50mm
       stepper.enableOutputs();
    }
stepper.run();

}
