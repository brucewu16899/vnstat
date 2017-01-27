#include "common.h"
#include "dbsql.h"
#include "dbxml.h"

void showxml(const char *interface, const char mode)
{
	interfaceinfo info;

	if (!db_getinterfacecountbyname(interface)) {
		return;
	}

	if (!db_getinterfaceinfo(interface, &info)) {
		return;
	}

	printf(" <interface name=\"%s\">\n", info.name);

	printf("  <name>%s</name>\n", info.name);
	printf("  <alias>%s</alias>\n", info.alias);

	printf("  <created>");
	xmldate(&info.created, 1);
	printf("</created>\n");
	printf("  <updated>");
	xmldate(&info.updated, 2);
	printf("</updated>\n");

	printf("  <traffic>\n");
	printf("   <total><rx>%"PRIu64"</rx><tx>%"PRIu64"</tx></total>\n", info.rxtotal, info.txtotal);

	/* TODO: add years and 5 minutes */
	switch (mode) {
		case 'd':
			xmldump(&info, "day", 1);
			break;
		case 'm':
			xmldump(&info, "month", 3);
			break;
		case 't':
			xmldump(&info, "top", 1);
			break;
		case 'h':
			xmldump(&info, "hour", 2);
			break;
		case 'a':
		default:
			xmldump(&info, "day", 1);
			xmldump(&info, "month", 3);
			xmldump(&info, "top", 1);
			xmldump(&info, "hour", 2);
			break;
	}

	printf("  </traffic>\n");
	printf(" </interface>\n");
}

void xmldump(const interfaceinfo *interface, const char *tablename, const int datetype)
{
	dbdatalist *datalist = NULL, *datalist_i = NULL;
	dbdatalistinfo datainfo;

	if (!db_getdata(&datalist, &datainfo, interface->name, tablename, -1)) {
		printf("Error: Failed to fetch %s data.\n", tablename);
		return;
	}

	printf("   <%ss>\n", tablename);
	datalist_i = datalist;
	while (datalist_i != NULL) {
		printf("    <%s id=\"%"PRId64"\">", tablename, datalist_i->rowid);
		xmldate(&datalist_i->timestamp, datetype);
		printf("<rx>%"PRIu64"</rx><tx>%"PRIu64"</tx></%s>\n", datalist_i->rx, datalist_i->tx, tablename);
		datalist_i = datalist_i->next;
	}
	dbdatalistfree(&datalist);
	printf("   </%ss>\n", tablename);
}

void xmldate(const time_t *date, const int type)
{
	struct tm *d;
	char *type1 = "<date><year>%d</year><month>%02d</month><day>%02d</day></date>";
	char *type2 = "<date><year>%d</year><month>%02d</month><day>%02d</day></date><time><hour>%02d</hour><minute>%02d</minute></time>";
	char *type3 = "<date><year>%d</year><month>%02d</month></date>";

	d = localtime(date);

	if (type == 1) {
		printf(type1, 1900+d->tm_year, 1+d->tm_mon, d->tm_mday);
	} else if (type == 2) {
		printf(type2, 1900+d->tm_year, 1+d->tm_mon, d->tm_mday, d->tm_hour, d->tm_min);
	} else if (type == 3) {
		printf(type3, 1900+d->tm_year, 1+d->tm_mon);
	}
}

void xmlheader(void)
{
	printf("<vnstat version=\"%s\" xmlversion=\"%d\">\n", getversion(), XMLVERSION);
}

void xmlfooter(void)
{
	printf("</vnstat>\n");
}
