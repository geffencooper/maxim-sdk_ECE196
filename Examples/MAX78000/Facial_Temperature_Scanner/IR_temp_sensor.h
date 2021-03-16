#ifndef IR_TEMP_SENSOR_H
#define IR_TEMP_SENSOR_H

/*
    Description: This function sets up the I2C driver

    Parameters: None

    Return: none
*/
void init_IR_temp_sensor();


/*
    Description: This function executes an I2C transaction to get the current temperature reading in Kelvin

    Parameters: none

    Return: Temperature in Kelvin
*/
float get_temp();

#endif