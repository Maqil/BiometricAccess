#include <inttypes.h>
#include <stdlib.h>
#include "mongoose.h"
#include "sqlite3.h"
#include "db_plugin.h"
#include "dllcall.h"
#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char * PUC;
typedef unsigned long UL;

struct client_infos {
	char id[100];
	char fname[100];
	char lname[100];
	char bdate[100];
	char y[100];
};

struct device_info {
	char devicename1[100];
	char deviceproprieties1[100];
	char devicenum1[100];
};

struct database_info {
	char usedrecords[100];
	char totalrecords[100];
	char freerecords[100];
};

struct identified_user {
	char userid[100];
	char firstname[100];
	char lastname[100];
};

char ** name = NULL;
char ** prop = NULL;

PUC	l_puc_fieldID;
PUC	l_puc_fieldFirstName;
PUC	l_puc_fieldSecondName;

UL	l_ul_fieldLenId;
UL	l_ul_fieldLenFirstName;
UL	l_ul_fieldLenSecondName;

int nb_dev;

char fnamefind[100];

static const char *s_http_port = "8080";
static struct mg_serve_http_opts s_http_server_opts;
static struct client_infos s_settings = { "mm", "ma", "we" , "r" , "r" };

static int s_sig_num = 0;
static void *s_db_handle = NULL;
static const char *s_db_path = "biometricacces.db";

static void handle_save(struct mg_connection *nc, struct http_message *hm) {

	void *cls = initMorphoDevice();

	/* Get form variables and store settings values */
	mg_get_http_var(&hm->body, "id", s_settings.id,
		sizeof(s_settings.id));
	mg_get_http_var(&hm->body, "fname", s_settings.fname,
		sizeof(s_settings.fname));
	mg_get_http_var(&hm->body, "lname", s_settings.lname,
		sizeof(s_settings.lname));
	mg_get_http_var(&hm->body, "bdate", s_settings.bdate,
		sizeof(s_settings.bdate));
	mg_get_http_var(&hm->body, "y", s_settings.y,
		sizeof(s_settings.y));

	printf("\nSettings updated to '%s' '%s' '%s' '%s' '%s'\n", s_settings.id, s_settings.fname,
		s_settings.lname, s_settings.bdate, s_settings.y);

	int ret = op_set_test(s_db_handle, s_settings.id, s_settings.fname, s_settings.lname, s_settings.bdate, s_settings.y);

	ret = callEnroll(cls, name, (unsigned char*)s_settings.id, (unsigned char*)s_settings.fname, (unsigned char*)s_settings.lname);

	/*op_get_test(nc, hm, s_db_handle, s_settings.fname);*/
	/*op_del_test(nc, hm, s_db_handle, s_settings.x);*/

	/* Send response */
	mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%.*s",
		(unsigned long)hm->body.len, (int)hm->body.len, hm->body.p);
}

static void handle_device(struct mg_connection *nc, const char *param) {

	if (strcmp(param, "devicename") == 0) {
		mg_printf_html_escape(nc, "%s", *name);
	}
	else if (strcmp(param, "deviceproprieties") == 0) {
		mg_printf_html_escape(nc, "%s", *prop);
	}
	else if (strcmp(param, "devicenum") == 0) {
		mg_printf_html_escape(nc, "%d", nb_dev);
	}

}

static void handle_databaseinfo(struct mg_connection *nc, const char *param) {

	int a = 15;
	int b = 500;
	int c = 486;

	if (strcmp(param, "usedrecords") == 0) {
		mg_printf_html_escape(nc, "%d", a);
	}
	else if (strcmp(param, "totalrecords") == 0) {
		mg_printf_html_escape(nc, "%d", b);
	}
	else if (strcmp(param, "freerecords") == 0) {
		mg_printf_html_escape(nc, "%d", c);
	}
}

static void handle_identified_user(struct mg_connection *nc, const char *param) {

	//l_puc_fieldID = (char*)malloc(sizeof(char*));
	//l_puc_fieldFirstName = (char*)malloc(sizeof(char*));
	//l_puc_fieldSecondName = (char*)malloc(sizeof(char*));

	char * userid = "24";
	char * userfn = "Mohamed";
	char * userln = "Maqil";

	if (strcmp(param, "userid") == 0) {
		mg_printf_html_escape(nc, "%s", userid);
	}
	else if (strcmp(param, "firstname") == 0) {
		mg_printf_html_escape(nc, "%s", userfn);
	}
	else if (strcmp(param, "lastname") == 0) {
		mg_printf_html_escape(nc, "%s", userln);
		
	}
}

static void handle_find(struct mg_connection *nc, struct http_message *hm) {
	/* Get form variables and store settings values */
	mg_get_http_var(&hm->body, "fname", fnamefind , sizeof(fnamefind));
	op_get_test(nc, hm, s_db_handle, fnamefind);
}

//static void identify_func(struct mg_connection *nc, struct http_message *hm) {
//	printf("\nIdentify page called");
//	//mg_send_file(nc, "identifier.shtml", NULL);
//}
//
//static void admin_func(struct mg_connection *nc, struct http_message *hm) {
//	printf("\nAdmin page called");
//}

static void signal_handler(int sig_num) {
	signal(sig_num, signal_handler);
	s_sig_num = sig_num;
}


static void ev_handler(struct mg_connection *nc, int ev, void *p) {

	struct http_message *hm = (struct http_message *) p;

	void *cls = initMorphoDevice();

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		if (mg_vcmp(&hm->uri, "/save") == 0) {
			handle_save(nc, hm);
		}
		else if (mg_vcmp(&hm->uri, "/find") == 0) {
			handle_find(nc, hm);
		}
		else if (mg_vcmp(&hm->uri, "/identify") == 0) {
			callIdentify(cls, name, l_puc_fieldID, l_puc_fieldFirstName, l_puc_fieldSecondName);
			printf("%u,%u,%u", l_puc_fieldID, l_puc_fieldFirstName, l_puc_fieldSecondName);
			handle_identified_user(nc, hm);
		}
		/*else if (mg_vcmp(&hm->uri, "/admin.shtml") == 0) {
			admin_func(nc, hm);
		}*/
		else if (mg_vcmp(&hm->uri, "/get_fingerprint") == 0) {
			printf("called");
		}
		else {
			mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
		}
		break;
	case MG_EV_SSI_CALL:

		if (nb_dev > 0) {
			printf("\n%d\n", nb_dev);
			handle_device(nc, p);
			
			
		}
		handle_identified_user(nc, hm);
		handle_databaseinfo(nc, p);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[]) {
	struct mg_mgr mgr;
	struct mg_connection *nc;
	cs_stat_t st;

	//unsigned long Cnb;
	//char * name ;
	//char * prop ;


	//printf("%s \n%s", name, prop);
	
	/* Open database */
	if ((s_db_handle = db_open(s_db_path)) == NULL) {
		fprintf(stderr, "Cannot open DB [%s]\n", s_db_path);
		exit(EXIT_FAILURE);
	}

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	mg_set_protocol_http_websocket(nc);

	if (nc == NULL) {
		fprintf(stderr, "Cannot bind to %s\n", s_http_port);
		exit(1);
	}

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = "BiometricWeb";
	s_http_server_opts.auth_domain = "example.com";

	if (mg_stat(s_http_server_opts.document_root, &st) != 0) {
		fprintf(stderr, "%s", "Cannot find BiometricWeb directory, exiting\n");
		exit(1);
	}

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	printf("Starting web server on port %s\n", s_http_port);

	//dllfunc();

		/*void * cls = initCPPClass();
		char ** cdata = NULL;
		int x = 0;
		int * count = &x;
		cdata = (char*)malloc(sizeof(char));
		CPPClassFillData(cls, cdata, count);
		printf("count: %d\n", *count);
		printf("cdata: %s", *cdata);
		getchar();
		releaseCPPClass(cls);*/

		name = (char*)malloc(sizeof(char*));
		prop = (char*)malloc(sizeof(char*));

		l_puc_fieldID = (unsigned long*)malloc(sizeof(unsigned long*));
		l_puc_fieldFirstName = (unsigned long*)malloc(sizeof(unsigned long*));
		l_puc_fieldSecondName = (unsigned long*)malloc(sizeof(unsigned long*));

		void *cls = initMorphoDevice();
		nb_dev = callUsbDevicesEnum(cls, name, prop);
		if (nb_dev > 0)
		{
			printf("\n******************Device Infos****************\n");
			printf("%s\n", *name);
			printf("%s", *prop);
			printf("\n**********************************************\n");
		}

		//unsigned long * us; 
		//unsigned long * tr;
		//unsigned long * fr;

		//us = (unsigned long)malloc(sizeof(unsigned long));
		//tr = (unsigned long)malloc(sizeof(unsigned long));
		//fr = (unsigned long)malloc(sizeof(unsigned long));


		callDatabaseInfo(cls, name/*us,tr,fr*/);

		//printf("%ld / %ld entries (%ld free).\n", us, tr, fr);

		releaseMorphoDevice(cls);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	/* Close database */
	db_close(&s_db_handle);
	system("pause");
	return 0;
}