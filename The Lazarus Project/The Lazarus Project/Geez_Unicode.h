//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Geez_Unicode.h
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		The following file defines the Unicodes of the Geez Fidel's as used
//						by the Windows OS.
//
// DATE CREATED:		8th of Oct 2017, Saturday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#ifndef GEEZ_H
#define GEEZ_H

#include "Lazarus.h"

// define the Geez unicode's here used by our classifer
short amharic_unicodes[NUM_PATTERNS] = {0x1200, 0x1201, 0x1202, 0x1203, 0x1204, 0x1205, 0x1206,		// HA - HO
										0x12B8, 0x12B9, 0x12BA, 0x12BB, 0x12BC, 0x12BD, 0x12BE,		// Haa - Hoo
										0x1208, 0x1209, 0x120A, 0x120B, 0x120C, 0x120D, 0x120E,		// La - Lo
										0x12C8, 0x12C9, 0x12CA, 0x12CB, 0x12CC, 0x12CD, 0x12CE,		// Wa- Wo
										0x1210, 0x1211, 0x1212, 0x1213, 0x1214, 0x1215, 0x1216,		// Hameru Ha - Ho
										0x12D0, 0x12D1, 0x12D2, 0x12D3, 0x12D4, 0x12D5, 0x12D6,		// Kibu Aa - Ao
										0x1218, 0x1219, 0x121A, 0x121B, 0x121C, 0x121D, 0x121E,		// Ma - Mo
										0x12D8, 0x12D9, 0x12DA, 0x12DB, 0x12DC, 0x12DD, 0x12DE,		// Za - Zo
										0x1220, 0x1221, 0x1222, 0x1223, 0x1224, 0x1225, 0x1226,		// Shukaw Sa - So
										0x12E0, 0x12E1, 0x12E2, 0x12E3, 0x12E4, 0x12E5, 0x12E6,		// Gze - Gzu
										0x1228, 0x1229, 0x122A, 0x122B, 0x122C, 0x122D, 0x122E,		// Ra - Ro
										0x12E8, 0x12E9, 0x12EA, 0x12EB, 0x12EC, 0x12ED, 0x12EE,		// Ya - Yo
										0x1230, 0x1231, 0x1232, 0x1233, 0x1234, 0x1235, 0x1236,		// Esatu Sa - So
										0x12F0, 0x12F1, 0x12F2, 0x12F3, 0x12F4, 0x12F5, 0x12F6,		// Da - Do
										0x1238, 0x1239, 0x123A, 0x123B, 0x123C, 0x123D, 0x123E,		// Sha - Sho
										0x1300, 0x1301, 0x1302, 0x1303, 0x1304, 0x1305, 0x1306,		// Ja - Jo
										0x1240, 0x1241, 0x1242, 0x1243, 0x1244, 0x1245, 0x1246,		// Qa - Qo
										0x1308, 0x1309, 0x130A, 0x130B, 0x130C, 0x130D, 0x130E,		// Ga - Go
										0x1260, 0x1261, 0x1262, 0x1263, 0x1264, 0x1265, 0x1266,		// Ba - Bo
										0x1320, 0x1321, 0x1322, 0x1323, 0x1324, 0x1325, 0x1326,		// Ta'a - To'o
										0x1270, 0x1271, 0x1272, 0x1273, 0x1274, 0x1275, 0x1276,		// Ta - To
										0x1328, 0x1329, 0x132A, 0x132B, 0x132C, 0x132D, 0x132E,		// Ch'ea - Ch'o
										0x1278, 0x1279, 0x127A, 0x127B, 0x127C, 0x127D, 0x127E,		// Cha - Cho
										0x1330, 0x1331, 0x1332, 0x1333, 0x1334, 0x1335, 0x1336,		// Pe'a - Pe'o
										0x1280, 0x1281, 0x1282, 0x1283, 0x1284, 0x1285, 0x1228,		// Thrd Ha - Ho
										0x1338, 0x1339, 0x133A, 0x133B, 0x133C, 0x133D, 0x133E,		// Tse - Tso
										0x1290, 0x1291, 0x1292, 0x1293, 0x1294, 0x1295, 0x1296,		// Na - No
										0x1340, 0x1341, 0x1342, 0x1343, 0x1344, 0x1345, 0x1346,		// Tseday Tse - Tso
										0x1298, 0x1299, 0x129A, 0x129B, 0x129C, 0x128D, 0x129E,		// Gne - Gno
										0x1348, 0x1349, 0x134A, 0x134B, 0x134C, 0x134D, 0x134E,		// Fa - Fo
										0x12A0, 0x12A1, 0x12A2, 0x12A3, 0x12A4, 0x12A5, 0x12A6,		// Aa - Ao
										0x1350, 0x1351, 0x1352, 0x1353, 0x1354, 0x1355, 0x1356,		// Pa - Po
										0x12A8, 0x12A9, 0x12AA, 0x12AB, 0x12AC, 0x12AD, 0x12AE};//,	// Ka - Ko
										//0x1268, 0x1269, 0x126A, 0x126B, 0x126C, 0x126D, 0x126E,		// Va - Vo
										//0x1250, 0x1251, 0x1252, 0x1253, 0x1254, 0x1255, 0x1256};	// Qua - Quo (ya know, the Tigirgna style of Qa - Qo
										//0x1318, 0x1319, 0x131A, 0x131B, 0x131C, 0x131D, 0x131E,		// Strange Ga - Go (May be gua - goo)


#endif

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|