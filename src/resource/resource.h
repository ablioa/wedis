#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDI_MAIN                                100
#define IDD_ABOUT                               108
#define ID_MAIN                                 109
#define IDB_TOOLBAR_MAIN                        110
#define IDB_CONNECTION                          111
#define IDD_PREFERENCE                          113
#define IDD_GOTOLINE                            114
#define IDD_DB_SEARCH                           115
#define IDB_TOOLBAR_STRINGTB                    116
#define IDB_TOOLBAR_LISTTB                      117
#define IDB_TOOLBAR_STATTB                      118
#define IDB_TOOLBAR_DATABASETB                  119
#define IDB_TOOLBAR_ZSETTB                      120
#define IDB_TOOLBAR_HASHTB                      121
#define IDB_TOOLBAR_SETTB                       122
#define IDB_TOOLBAR                             123
#define IDC_CONNECTION_HOSTVALUE                201
#define IDC_CONNECTION_PORTVALUE                203
#define IDC_CONNECTION_IFPASSWORD               204
#define IDC_CONNECTION_PASSWORDVALUE            206
#define IDC_CONNECTION_VALUE                    208
#define IDC_CONNECTION_SPLIT                    216
#define IDC_CONNECTION_TREE                     219
#define IDC_CONNECTION_DESCRIPTION              220
#define IDC_CONFIG_CONNECT                      221
#define IDD_ADD_ENTRY                           300
#define IDD_CONNECTION                          400
#define IDC_CONFIG_DONE                         401
#define ID_CONFIG_CANCEL                        402
#define IDC_PREFERENCE_SCAN_COUNT               403
#define IDC_ABOUT_OK                            500
#define IDD_LIST_ITEM                           600
#define IDC_LIST_STATIC                         601
#define IDC_LIST_OK                             602
#define IDC_LIST_CANCEL                         603
#define IDM_STAT_SERVER                         700
#define IDM_STAT_CLIENT                         701
#define IDM_STAT_MEMORY                         702
#define IDM_STAT_PERSISENCE                     703
#define IDM_STAT_STATS                          704
#define IDM_STAT_REPLICATION                    705
#define IDM_STAT_CPU                            706
#define IDM_STAT_CLUSTER                        707
#define IDM_STAT_KEYSPACE                       708
#define IDM_CONNECTION_POOL                     899
#define IDM_EXIT                                4002
#define IDM_ABOUT                               4004
#define IDA_RUN                                 4014
#define IDM_EXE_RUN                             4015
#define IDM_EXE_BRK                             4030
#define IDB_CHIP                                5031
#define IDC_CONNECTION_ADD                      40000
#define ID_COLOR_SPLITBAR                       40000
#define IDC_CONNECTION_DELETE                   40001
#define ID_COLOR_CURLINE                        40001
#define IDC_CONNECTION_TEST                     40002
#define IDS_DATAVIEW                            40002
#define IDS_CODEVIEW                            40003
#define IDS_REGVIEW                             40004
#define IDS_STACKVIEW                           40005
#define IDS_HOSTLIST                            40006

#define IDS_TB_REFRESH_SET_DATA                 40007
#define IDS_TB_MOVE_SET_DATA                    40008
#define IDS_TB_DELETE_SET_DATA                  40009
#define IDS_TB_ADD_DATA_ITEM                    40010
#define IDS_TB_DELETE_DATA_ITEM                 40011

#define IDS_TB_MAIN_CONNECTION                  40012 
#define IDS_TB_MAIN_PREFERENCE                  40013
#define IDM_PREFERENCE                          40014

#define IDS_TB_REFRESH_LIST_DATA                40015 
#define IDS_TB_MOVE_LIST_DATA                   40016
#define IDS_TB_DELETE_LIST_DATA                 40017
#define IDS_TB_EXPORT_LIST_DATA                 40018

#define IDS_TB_SYS_FLUSH_ALL                    40019          
#define IDS_TB_SYS_SERVER                       40020
#define IDS_TB_SYS_CLIENTS                      40021
#define IDS_TB_SYS_MEMORY                       40022
#define IDS_TB_SYS_PERSISTENCE                  40023
#define IDS_TB_SYS_STATS                        40024
#define IDS_TB_SYS_REPLICATION                  40025
#define IDS_TB_SYS_CPU                          40026
#define IDS_TB_SYS_MODULES                      40027
#define IDS_TB_SYS_CLUSTER                      40028
#define IDS_TB_SYS_KEYSPACE                     40029

#define IDS_TB_STR_REFRESH_DATA                 40030
#define IDS_TB_STR_MOVE_DATA                    40031
#define IDS_TB_STR_DELETE_DATA                  40032
#define IDS_TB_STR_EXPORT_DATA                  40033
#define IDS_TB_STR_FORMAT_DATA                  40034
#define IDS_TB_STR_WIDEN_DATA                   40035

#define IDE1 50001
#define IDE2 50002
#define IDE3 50003

#define IDC_DATATYPE         1017
#define IDC_DB_SEARCH_CANCEL 40012
#define IDC_DB_SEARCH_OK     40013

/** common toolbar command */
#define TB_CMD_REFRESH_DATA 4000
#define TB_CMD_DELETE_DATA  4001
#define TB_CMD_MOVE_DATA    4002
#define TB_CMD_EXPORT_DATA  4003
#define TB_CMD_FORMAT_DATA  4004
#define TB_CMD_WIDEN_DATA   4005
#define TB_CMD_FLUSH_DB     4006
#define TB_CMD_ADD_DATA     4007

/** total image list icons */
#define TOTAL_ICON_COUNT 22

/** defines toolbar button id */
#define TB_DELETE_BUTTON 0
#define TB_EXPORT_BUTTON 10
#define TB_FORMAT_BUTTON 11
#define TB_MOVE_BUTTON   12
#define TB_CONNECTION_BUTTON   13
#define TB_PREFERENCE_BUTTON   14
/** redis tree node icon */
#define TREE_HOST_NODE       15
#define TREE_DATABASE_NODE   16
#define TREE_DATA_NODE       17
#define TREE_FOLDER_NODE     18
#define TB_WIDEN_BUTTON      19
#define TB_REFRESH_BUTTON    20
#define TB_ADD_BUTTON        21
