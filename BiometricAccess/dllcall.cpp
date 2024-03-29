#include "MORPHO_Types.h"
#include "MORPHO_TemplateList.h"
#include "MORPHO_FieldList.h"
#include "MORPHO_User.h"
#include "MORPHO_UserList.h"
#include "MORPHO_FieldDescriptor.h"
#include "MORPHO_Database.h"
#include "MORPHO_Device.h"
#include "dllcall.h"
#include <iostream>

using namespace std;

int dllfunc()
{
	C_MORPHO_Device device;
	
	UL nb;
	char * name;
	char * prop;

	//device.InitUsbDevicesEnum(&nb);
	int res = device.InitUsbDevicesNameEnum(&nb);
	cout << nb << endl;
	
	res = device.GetUsbDevicesNameEnum(0, name, prop);
	
	cout << name << endl;
	cout << prop << endl;

    return 0;
}

