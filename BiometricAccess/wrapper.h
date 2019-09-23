#pragma once
#ifdef __cplusplus
extern "C" {
#endif
	void * initMorphoDevice();
	void releaseMorphoDevice(void* cls);
	int callUsbDevicesEnum(void * cls,char ** nm, char ** prp);
	//int CPPClassFillData(void * cls, char ** data, int * count);
	void callIdentify(void * cls, char ** nm,unsigned char * l_puc_fieldID , unsigned char * l_puc_fieldFirstName, unsigned char * l_puc_fieldSecondName);
	int callEnroll(void * cls, char ** nm, unsigned char* id, unsigned char* fna, unsigned char* lna);
	void callDatabaseInfo(void * cls, char ** nm/*, unsigned long l_ul_usedRecords, unsigned long l_ul_freeRecords, unsigned long l_ul_totalRecords*/);
#ifdef __cplusplus
}
#endif


