#ifndef AIR_SETTIGS_H
#define AIR_SETTIGS_H

#include "storage/EEPROMconfig.h"
#include "storage/Storage.h"
#include "util/LTATune.h"

namespace AirSettings{
    //note: These settings need to match the dashboard's resource_air file
	enum Air{
		INT_PERIOD =  0,
		INRT_U_FAC =  1,
		GYRO_CMP_F =  2,
		TILT_CMP_L =  3,
		ATT_P_TERM =  4,
		ATT_I_TERM =  5,
		ATT_D_TERM =  6,
		ATT_VP_TERM=  7,
		ATT_VI_TERM=  8,
		ATT_VD_TERM=  9,
		YAW_P_TERM = 10,
		YAW_I_TERM = 11,
		YAW_D_TERM = 12,
		YAW_V_TERM = 13,
		HOVER_THL  = 14,
		THL_LINITY = 15,
		BARO_HL    = 16,
		BARO_P     = 17,
		BARO_I     = 18,
		BARO_D     = 19,
		BARO_V     = 20,
		FREE_START = 21
	};
}
namespace groundSettings{
	//note: These settings need to match the dashboard's resource_ground file
	enum Ground{
		LINE_GRAV	=  0,
		STEER_THROW	=  1,
		STEER_STYLE	=  2,
		STEER_FAC	=  3,
		MIN_FWD_SPD	=  4,
		MAX_FWD_SPD	=  5,
		REV_STR_THR	=  6,
		MAX_REV_SPD	=  7,
		PING_WEIGHT	=  8,
		COAST_TIME	=  9,
		MIN_REV_T	= 10,
		CRUISE_P	= 11,
		CRUISE_I	= 12,
		CRUISE_D	= 13,
		TIRE_DIAM	= 14,
		STR_CENTER	= 15,
		FREE_START  = 16
	};
}
namespace commonSettings{
	static const uint16_t VERSION =	 __TIME__[6]
									+__TIME__[7]*10
									+__TIME__[3]*100
									+__TIME__[4]*1000;
	static const uint16_t CALIBRATION_VERSON = 8;
	enum Common{
		ACCL_X_SHFT	= 50,
		ACCL_Y_SHFT	= 51,
		ACCL_Z_SHFT	= 52,
		ACCL_X_SCLR	= 53,
		ACCL_Y_SCLR	= 54,
		ACCL_Z_SCLR	= 55,
		MAG_X_SHFT	= 56,
		MAG_Y_SHFT	= 57,
		MAG_Z_SHFT	= 58,
		MAG_X_SCLR	= 59,
		MAG_Y_SCLR	= 60,
		MAG_Z_SCLR	= 61,
		CALIB_VER	= 62,
		STORAGE_VER	= 63
	};
}
using namespace commonSettings;
class Settings{
	// makes the usage of Storage for settings easier to top level sketches
	// attach is a single action
	// keeps track of weather or not storage was initialized
	// structured and safe retreival of 2 LTATunes from memory
private:
	Storage<EE_STORAGE_TYPE> *storage = NULL;
	bool formatChecked = false;
	bool validFormat = false;
	bool validCalib  = false;
public:
	Settings(Storage<EE_STORAGE_TYPE> *str) : storage(str) {
	}
	void checkStorageFormat(){
		if(storage == NULL) return;
		validCalib  = (storage->getRecord(CALIB_VER  ) == CALIBRATION_VERSON);
		validFormat = (storage->getRecord(STORAGE_VER) == VERSION);
		if (!validFormat) storage->updateRecord(STORAGE_VER, VERSION);
		formatChecked = true;
	}
	void writeCalibrationVersion(){
		if(storage == NULL) return;
		storage->updateRecord(CALIB_VER, CALIBRATION_VERSON);
	}
	void writeTuningValues(LTATune accel, LTATune mag){
		if(storage == NULL) return;
		for(int i=0; i<6; i++){
			storage->updateRecord(ACCL_X_SHFT+i, accel.raw[i]);
			storage->updateRecord( MAG_X_SHFT+i,   mag.raw[i]);
		}
		writeCalibrationVersion();
	}
	bool attach(int type, EE_STORAGE_TYPE defaul, void (*call)(EE_STORAGE_TYPE)){
		if(!formatChecked) checkStorageFormat();
		if(storage == NULL) return false;
		uint8_t index = (int)type;

		if (!validFormat) {
			storage->updateRecord(index, defaul);
		}
		storage->attachCallback(index, call);
		return true;
	}
	float get(int type){
		uint8_t index = (int)type;
		return storage->getRecord(index);
	}
	LTATune getTuneAt(int startIndex){
		if(!formatChecked) checkStorageFormat();
		if(storage == NULL) return LTATune();
		LTATune output;
		if(!validCalib) return output;
		for(int i=0; i<6; i++){
			output.raw[i] = storage->getRecord(startIndex+i);
		}
		return output;
	}
	LTATune getAccelTune(){
		if(!formatChecked) checkStorageFormat();
		return getTuneAt(ACCL_X_SHFT);
	}
	LTATune getMagTune(){
		if(!formatChecked) checkStorageFormat();
		return getTuneAt( MAG_X_SHFT);
	}
};

#endif
