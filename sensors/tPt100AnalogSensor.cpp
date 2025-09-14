/*
 * tPt100AnalogSensor.cpp
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tPt100AnalogSensor.h"

#if CONFIG_PT100_ANALOG_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_AnalogSensor_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "AnalogInput";
	return IdPrefix;
}

uint8_t tSensorJsonFormatter_AnalogSensor_api_1::FormatJSON(Stream *pStream, tSensorCache *cache){
   if (cache->getDataBlobSize() != sizeof(tPt100AnalogSensorTypes::tResult_api_v1))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tPt100AnalogSensorTypes::tResult_api_v1 *pResult =
		   (tPt100AnalogSensorTypes::tResult_api_v1 *) cache->getData();
   pStream->print(F("\"Temperature\":"));
   pStream->print(pResult->Temperature);
   return STATUS_SUCCESS;
}
#endif CONFIG_PT100_ANALOG_SENSOR_JSON_OUTPUT



#if CONFIG_PT100_ANALOG_SENSOR


//const float pt100Table[] PROGMEM = {
///*0,*/100.000,100.391,100.781,101.172,101.562,101.953,102.343,102.733,103.123,103.513,
///*10,*/103.903,104.292,104.682,105.071,105.460,105.849,106.238,106.627,107.016,107.405,
///*20,*/107.794,108.182,108.570,108.959,109.347,109.735,110.123,110.510,110.898,111.286,
///*30,*/111.673,112.060,112.447,112.835,113.221,113.608,113.995,114.382,114.768,115.155,
///*40,*/115.541,115.927,116.313,116.699,117.085,117.470,117.856,118.241,118.627,119.012,
///*50,*/119.397,119.782,120.167,120.552,120.936,121.321,121.705,122.090,122.474,122.858,
///*60,*/123.242,123.626,124.009,124.393,124.777,125.160,125.543,125.926,126.309,126.692,
///*70,*/127.075,127.458,127.840,128.223,128.605,128.987,129.370,129.752,130.133,130.515,
///*80,*/130.897,131.278,131.660,132.041,132.422,132.803,133.184,133.565,133.946,134.326,
///*90,*/134.707,135.087,135.468,135.848,136.228,136.608,136.987,137.367,137.747,138.126,
///*100,*/138.505,138.885,139.264,139.643,140.022,140.400,140.779,141.158,141.536,141.914,
///*110,*/142.293,142.671,143.049,143.426,143.804,144.182,144.559,144.937,145.314,145.691,
///*120,*/146.068,146.445,146.822,147.198,147.575,147.951,148.328,148.704,149.080,149.456,
///*130,*/149.832,150.208,150.583,150.959,151.334,151.710,152.085,152.460,152.835,153.210,
///*140,*/153.584,153.959,154.333,154.708,155.082,155.456,155.830,156.204,156.578,156.952,
///*150,*/157.325,157.699,158.072,158.445,158.818,159.191,159.564,159.937,160.309,160.682,
///*160,*/161.054,161.427,161.799,162.171,162.543,162.915,163.286,163.658,164.030,164.401,
///*170,*/164.772,165.143,165.514,165.885,166.256,166.627,166.997,167.368,167.738,168.108,
///*180,*/168.478,168.848,169.218,169.588,169.958,170.327,170.696,171.066,171.435,171.804,
///*190,*/172.173,172.542,172.910,173.279,173.648,174.016,174.384,174.752,175.120,175.488,
///*200,*/175.856,176.224,176.591,176.959,177.326,177.693,178.060,178.427,178.794,179.161,
///*210,*/179.528,179.894,180.260,180.627,180.993,181.359,181.725,182.091,182.456,182.822,
///*220,*/183.188,183.553,183.918,184.283,184.648,185.013,185.378,185.743,186.107,186.472,
///*230,*/186.836,187.200,187.564,187.928,188.292,188.656,189.019,189.383,189.746,190.110,
///*240,*/190.473,190.836,191.199,191.562,191.924,192.287,192.649,193.012,193.374,193.736,
///*250,*/194.098,194.460,194.822,195.183,195.545,195.906,196.268,196.629,196.990,197.351,
///*260,*/197.712,198.073,198.433,198.794,199.154,199.514,199.875,200.235,200.595,200.954,
///*270,*/201.314,201.674,202.033,202.393,202.752,203.111,203.470,203.829,204.188,204.546,
///*280,*/204.905,205.263,205.622,205.980,206.338,206.696,207.054,207.411,207.769,208.127,
///*290,*/208.484,208.841,209.198,209.555,209.912,210.269,210.626,210.982,211.339,211.695,
///*300,*/212.052,212.408,212.764,213.120,213.475,213.831,214.187,214.542,214.897,215.252
//};

// GOOD ENOUGH PRECISION FOR ME = 0.38ohm / dec
// Temp = (ohm-100) *  1/0.38 = (ohm-100) *  2,63

/*
 *
 * +5V
 *        |
 *        R1
 *        |
 *        |               |\
 *        |---------------|+\
 *        |               |  --------------R 22k------ Analog In
 *        |             --|-/   |
 *      PT100           | |/    |
 *        |             |       |
 *        |             --- R4---
 *        |             |
 *       GND            R3
 *                      |
 *                     GND
 *
 *
 *
 *
 *
 *
 *
 */


//static const float R1 = 5000;
//static const float R3 = 3300;
//static const float R4 = 30000;
//static const float AMPL = 1+(R4/R3);  = 10.1
//
// U Analog in = 5 * AMPL * PT100 / (R1 + Pt100)

//200 = 383
//150 = 288
//100 = 194

// R pt = AnalogRead * 0.529 - 3;



void tPt100AnalogSensor::doTimeTick()
{
   int Read = analogRead(Config.Pin);

   float ohm = ((float)Read * 0.529) - 3 - Config.Correction;

   float Temp = (ohm-100) * 2.63;
   TemperatureAvg = TemperatureAvg * 0.9 + Temp*0.1;
}

void tPt100AnalogSensor::doTriggerMeasurement()
{
   if (TemperatureAvg < 500)
   {
      mResult.Temperature = round (TemperatureAvg);
      onMeasurementCompleted(true);
   }
   else
   {
      onMeasurementCompleted(false);
   }
}

#endif //CONFIG_PT100_ANALOG_SENSOR

#if CONFIG_SENSOR_LOGGER

void tPt100SensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tPt100AnalogSensorTypes::tResult_api_v1 *pResult =
    		(tPt100AnalogSensorTypes::tResult_api_v1 *)pDataBlob;

    LOG_PRINT("PT100 Measurement completed. SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" Temp: ");
    LOG(println(pResult->Temperature));
}

#endif CONFIG_SENSOR_LOGGER
