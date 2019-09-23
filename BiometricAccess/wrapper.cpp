//#include "wrapper.h"
//#include "CPPClass.h"
//#include "MORPHO_Types.h"
//#include "MORPHO_TemplateList.h"
//#include "MORPHO_FieldList.h"
//#include "MORPHO_User.h"
//#include "MORPHO_UserList.h"
//#include "MORPHO_FieldDescriptor.h"
//#include "MORPHO_Database.h"
//#include "MORPHO_Device.h"
//#include "MORPHO_Errors.h"
#include <iostream>
using namespace std;

typedef unsigned long    UL;
typedef unsigned char    UC;

#define MAX_FIELD_SIZE 64
#define MAX_FIELD_NAME_LEN 6

//return a void pointer,
//this is a CPPClass object instanstiated using new.
extern "C" void* initMorphoDevice()
{
    //cout << " sdf" << endl;
    return new C_MORPHO_Device;
}

extern "C" void* initMorphoDatabase()
{
    return new C_MORPHO_Database;
}



//the initialized void  pointer will be casted to
//CPPClass and then deleted using its destructor.
extern "C" void releaseMorphoDevice(void* cls) {
    delete static_cast<C_MORPHO_Device*>(cls);
}

extern "C" int callUsbDevicesEnum(void * cls,char ** nm, char ** prp)
{
    C_MORPHO_Device * device = static_cast<C_MORPHO_Device *> (cls);
    UL count;

    /*C_MORPHO_Device device;*/
    int ret = device->InitUsbDevicesNameEnum(&count);
    if (ret == 0 && count > 0)
    {

        //just to test
        ret = device->GetUsbDevicesNameEnum((UL)0, *nm,*prp);
        if (ret == 0)
        {
            //cout << nm << endl;
            //cout << prp << endl;
        }
    }
    return count;
}

void callDatabaseInfo(void * cls, char ** nm/*, unsigned long l_ul_usedRecords, unsigned long l_ul_freeRecords, unsigned long l_ul_totalRecords*/)
{
    C_MORPHO_Device * device = static_cast<C_MORPHO_Device *> (cls);
    C_MORPHO_Database        m_x_database;
    int res;

    res = device->OpenUsbDevice(*nm, 0, NULL);

    res = device->GetDatabase(0, NULL, m_x_database);
    if (res == 0)
    {
        UL    l_ul_usedrecords;
        UL    l_ul_freerecords;
        UL    l_ul_totalrecords;
        fprintf(stdout, "\n******************Database Infos****************\n");
        fprintf(stdout, "\nDatabase found on the device\n");
        m_x_database.GetNbUsedRecord(l_ul_usedrecords);
        m_x_database.GetNbFreeRecord(l_ul_freerecords);
        m_x_database.GetNbTotalRecord(l_ul_totalrecords);
        fprintf(stdout, "%ld / %ld entries (%ld free).\n", l_ul_usedrecords, l_ul_totalrecords, l_ul_freerecords);
        fprintf(stdout,"\n**********************************************\n");
    }

}


extern "C" void callIdentify(void * cls, char ** nm, unsigned char * l_puc_fieldID1, unsigned char * l_puc_fieldFirstName1, unsigned char * l_puc_fieldSecondName1) {

    C_MORPHO_Device * device = static_cast<C_MORPHO_Device *> (cls);
    int res;
    C_MORPHO_Database        m_x_database;
    T_MORPHO_FAR l_x_matchingThreshold = MORPHO_FAR_5;
    C_MORPHO_User m_x_user;
    UL    l_ul_matchingScore;
    UC    l_uc_matchedFingerIndex;

    PUC    l_puc_fieldID;
    PUC    l_puc_fieldFirstName;
    PUC    l_puc_fieldSecondName;

    res = device->OpenUsbDevice(*nm, 0, NULL);

    res = device->GetDatabase(0, NULL, m_x_database);
    //if (res == 0)
    //{
    //    UL    l_ul_usedRecords;
    //    UL    l_ul_freeRecords;
    //    UL    l_ul_totalRecords;
    //    fprintf(stdout, "\nDatabase found on the device\n");
    //    m_x_database.GetNbUsedRecord(l_ul_usedRecords);
    //    m_x_database.GetNbFreeRecord(l_ul_freeRecords);
    //    m_x_database.GetNbTotalRecord(l_ul_totalRecords);
    //    fprintf(stdout, "%ld / %ld entries (%ld free).\n", l_ul_usedRecords, l_ul_totalRecords, l_ul_freeRecords);
    //}

    res = m_x_database.Identify(
        0,    //US                            i_us_Timeout,
        l_x_matchingThreshold,    //T_MORPHO_FAR                i_us_FAR,
        0,    //UL                            i_ul_CallbackCmd,
        NULL,    //T_MORPHO_CALLBACK_FUNCTION    i_pf_Callback,
        NULL,    //PVOID                        i_pv_CallbackArgument,
        m_x_user,    //C_MORPHO_User &                o_x_User,
        &l_ul_matchingScore,    //PUL                            o_pul_MatchingScore,
        &l_uc_matchedFingerIndex,//PUC                            o_puc_FingerIndex,
        MORPHO_DEFAULT_CODER,    //I                            i_i_CoderChoice,
        MORPHO_ENROLL_DETECT_MODE,    //UL                            i_ul_DetectModeChoice,
        MORPHO_STANDARD_MATCHING_STRATEGY,    //UL                            i_ul_MatchertModeChoice
        0
    );
    if (res == MORPHO_OK) {

        UL    l_ul_fieldLenId;
        UL    l_ul_fieldLenFirstName;
        UL    l_ul_fieldLenSecondName;

        UC    l_auc_stringBuffer[MAX_FIELD_SIZE];
        UL    l_ul_stats;
        UC    l_auc_fieldName[MAX_FIELD_NAME_LEN + 1];
        fprintf(stdout, "***********************************************************************\n");
        fprintf(stdout, "Match found (template %d). User info:\n", l_uc_matchedFingerIndex);
        m_x_user.GetField(0, l_ul_fieldLenId, l_puc_fieldID);
        m_x_user.GetField(1, l_ul_fieldLenFirstName, l_puc_fieldFirstName);
        m_x_user.GetField(2, l_ul_fieldLenSecondName, l_puc_fieldSecondName);

        fprintf(stdout, "\t\tUser id         #%.*s\n", l_ul_fieldLenId, l_puc_fieldID);
        fprintf(stdout, "\t\tUser First Name #%.*s\n", l_ul_fieldLenFirstName, l_puc_fieldFirstName);
        fprintf(stdout, "\t\tUser Last Name  #%.*s\n", l_ul_fieldLenSecondName, l_puc_fieldSecondName);
        fprintf(stdout, "***********************************************************************");

        l_puc_fieldID1 = (UC*)l_puc_fieldID;
        l_puc_fieldFirstName1 = (UC*)l_puc_fieldFirstName;
        l_puc_fieldSecondName1 = (UC*)l_puc_fieldSecondName;

    }

}

extern "C" int callEnroll(void * cls, char ** nm, unsigned char* id, unsigned char* fna, unsigned char* lna ) {

    int res;

    C_MORPHO_Device * device = static_cast<C_MORPHO_Device *> (cls);
    C_MORPHO_Database        m_x_database;

    res = device->OpenUsbDevice(*nm, 0, NULL);

    res = device->GetDatabase(0, NULL, m_x_database);

    /************enroll*********/
    T_MORPHO_COMPRESS_ALGO    l_x_compressAlgo = MORPHO_NO_COMPRESS;
    C_MORPHO_TemplateList    l_x_templates;
    l_x_templates.SetActiveFullImageRetrieving(true);
    l_x_templates.GetActiveFullImageRetrieving();
    PT_MORPHO_MOC_PARAMETERS l_px_MocParameters = NULL;
    C_MORPHO_User m_x_user;

    res = m_x_database.GetUser(2, id, m_x_user);

    res = m_x_user.PutField(1, 10, fna);
    res = m_x_user.PutField(2, 10, lna);

    //res = m_x_database.GetUser(3,(UC*)"203",m_x_user);

    //res = m_x_user.PutField(1, 10, (UC*)"rtrt");
    //res = m_x_user.PutField(2, 10, (UC*)"tyty");
    /**********enroll***********/

    res = m_x_user.Enroll(
        (US)0,
        (UC)0,
        (UC)0,
        l_x_compressAlgo,
        (UC)0,
        (UC)1,
        (UC)1,
        MORPHO_PK_COMP,
        MORPHO_NO_PK_FVP,
        (UC)1,
        NULL,
        NULL,
        NULL,
        0,
        (UC)(18 | 0),
        l_x_templates,
        l_px_MocParameters
    );
    if (res == 0) {
        cout << res << endl;
        cout << "\nEnrolled succesfuly" << endl;
    }
    return res;
}

