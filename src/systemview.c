#include "systemview.h"

#define NOTE_COUNT 181
typedef struct note_table{
    int          noteid;
    const char * name;
    char       * message;
}NoteTable;

NoteTable notes[NOTE_COUNT]={
    {IDS_NOTE_SERVER_REDIS_VERSION ,"redis_version",NULL},
    {IDS_NOTE_SERVER_REDIS_GIT_SHA1 ,"redis_git_sha1",NULL},
    {IDS_NOTE_SERVER_REDIS_GIT_DIRTY ,"redis_git_dirty",NULL},
    {IDS_NOTE_SERVER_REDIS_BUILD_ID ,"redis_build_id",NULL},
    {IDS_NOTE_SERVER_REDIS_MODE ,"redis_mode",NULL},
    {IDS_NOTE_SERVER_OS ,"os",NULL},
    {IDS_NOTE_SERVER_ARCH_BITS ,"arch_bits",NULL},
    {IDS_NOTE_SERVER_MULTIPLEXING_API ,"multiplexing_api",NULL},
    {IDS_NOTE_SERVER_ATOMICVAR_API ,"atomicvar_api",NULL},
    {IDS_NOTE_SERVER_GCC_VERSION ,"gcc_version",NULL},
    {IDS_NOTE_SERVER_PROCESS_ID ,"process_id",NULL},
    {IDS_NOTE_SERVER_PROCESS_SUPERVISED ,"process_supervised",NULL},
    {IDS_NOTE_SERVER_RUN_ID ,"run_id",NULL},
    {IDS_NOTE_SERVER_TCP_PORT ,"tcp_port",NULL},
    {IDS_NOTE_SERVER_SERVER_TIME_IN_USEC ,"server_time_in_usec",NULL},
    {IDS_NOTE_SERVER_UPTIME_IN_SECONDS ,"uptime_in_seconds",NULL},
    {IDS_NOTE_SERVER_UPTIME_IN_DAYS ,"uptime_in_days",NULL},
    {IDS_NOTE_SERVER_HZ ,"hz",NULL},
    {IDS_NOTE_SERVER_CONFIGURED_HZ ,"configured_hz",NULL},
    {IDS_NOTE_SERVER_LRU_CLOCK ,"lru_clock",NULL},
    {IDS_NOTE_SERVER_EXECUTABLE ,"executable",NULL},
    {IDS_NOTE_SERVER_CONFIG_FILE ,"config_file",NULL},
    {IDS_NOTE_CLIENT_CONNECTED_CLIENTS,"connected_clients",NULL},
    {IDS_NOTE_CLIENT_CLUSTER_CONNECTIONS,"cluster_connections",NULL},
    {IDS_NOTE_CLIENT_MAXCLIENTS,"maxclients",NULL},
    {IDS_NOTE_CLIENT_CLIENT_LONGEST_OUTPUT_LIST,"client_longest_output_list",NULL},
    {IDS_NOTE_CLIENT_CLIENT_BIGGEST_INPUT_BUF,"client_biggest_input_buf",NULL},
    {IDS_NOTE_CLIENT_BLOCKED_CLIENTS,"blocked_clients",NULL},
    {IDS_NOTE_CLIENT_TRACKING_CLIENTS,"tracking_clients",NULL},
    {IDS_NOTE_CLIENT_CLIENTS_IN_TIMEOUT_TABLE,"clients_in_timeout_table",NULL},
    {IDS_NOTE_CLIENT_IO_THREADS_ACTIVE,"io_threads_active",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY,"used_memory",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_HUMAN,"used_memory_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_RSS,"used_memory_rss",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_RSS_HUMAN,"used_memory_rss_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK,"used_memory_peak",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK_HUMAN,"used_memory_peak_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK_PERC,"used_memory_peak_perc",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_OVERHEAD,"used_memory_overhead",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_STARTUP,"used_memory_startup",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_DATASET,"used_memory_dataset",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_DATASET_PERC,"used_memory_dataset_perc",NULL},
    {IDS_NOTE_MEMORY_TOTAL_SYSTEM_MEMORY,"total_system_memory",NULL},
    {IDS_NOTE_MEMORY_TOTAL_SYSTEM_MEMORY_HUMAN,"total_system_memory_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_LUA,"used_memory_lua",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_LUA_HUMAN,"used_memory_lua_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_SCRIPTS,"used_memory_scripts",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_SCRIPTS_HUMAN,"used_memory_scripts_human",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY,"maxmemory",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY_HUMAN,"maxmemory_human",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY_POLICY,"maxmemory_policy",NULL},
    {IDS_NOTE_MEMORY_MEM_FRAGMENTATION_RATIO,"mem_fragmentation_ratio",NULL},
    {IDS_NOTE_MEMORY_MEM_FRAGMENTATION_BYTES,"mem_fragmentation_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_FRAG_RATIO,"allocator_frag_ratio",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_FRAG_BYTES,"allocator_frag_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RSS_RATIO,"allocator_rss_ratio",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RSS_BYTES,"allocator_rss_bytes",NULL},
    {IDS_NOTE_MEMORY_RSS_OVERHEAD_RATIO,"rss_overhead_ratio",NULL},
    {IDS_NOTE_MEMORY_RSS_OVERHEAD_BYTES,"rss_overhead_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_ALLOCATED,"allocator_allocated",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_ACTIVE,"allocator_active",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RESIDENT,"allocator_resident",NULL},
    {IDS_NOTE_MEMORY_MEM_NOT_COUNTED_FOR_EVICT,"mem_not_counted_for_evict",NULL},
    {IDS_NOTE_MEMORY_MEM_CLIENTS_NORMAL,"mem_clients_normal",NULL},
    {IDS_NOTE_MEMORY_MEM_CLIENTS_SLAVES,"mem_clients_slaves",NULL},
    {IDS_NOTE_MEMORY_MEM_AOF_BUFFER,"mem_aof_buffer",NULL},
    {IDS_NOTE_MEMORY_MEM_REPLICATION_BACKLOG,"mem_replication_backlog",NULL},
    {IDS_NOTE_MEMORY_MEM_TOTAL_REPLICATION_BUFFERS,"mem_total_replication_buffers",NULL},
    {IDS_NOTE_MEMORY_MEM_ALLOCATOR,"mem_allocator",NULL},
    {IDS_NOTE_MEMORY_ACTIVE_DEFRAG_RUNNING,"active_defrag_running",NULL},
    {IDS_NOTE_MEMORY_LAZYFREE_PENDING_OBJECTS,"lazyfree_pending_objects",NULL},
    {HDS_NOTE_STAT_TOTAL_CONNECTIONS_RECEIVED,"total_connections_received",NULL}  ,  
    {HDS_NOTE_STAT_TOTAL_COMMANDS_PROCESSED,"total_commands_processed",NULL}    ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_OPS_PER_SEC,"instantaneous_ops_per_sec",NULL}   ,  
    {HDS_NOTE_STAT_TOTAL_NET_INPUT_BYTES,"total_net_input_bytes",NULL}       ,  
    {HDS_NOTE_STAT_TOTAL_NET_OUTPUT_BYTES,"total_net_output_bytes",NULL}      ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_INPUT_KBPS,"instantaneous_input_kbps",NULL}    ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_OUTPUT_KBPS,"instantaneous_output_kbps",NULL}   ,  
    {HDS_NOTE_STAT_REJECTED_CONNECTIONS,"rejected_connections",NULL}        ,  
    {HDS_NOTE_STAT_SYNC_FULL,"sync_full",NULL}                   ,  
    {HDS_NOTE_STAT_SYNC_PARTIAL_OK,"sync_partial_ok",NULL}             ,  
    {HDS_NOTE_STAT_SYNC_PARTIAL_ERR,"sync_partial_err",NULL}            ,  
    {HDS_NOTE_STAT_EXPIRED_KEYS,"expired_keys",NULL}                ,  
    {HDS_NOTE_STAT_EXPIRED_STALE_PERC,"expired_stale_perc",NULL}          ,  
    {HDS_NOTE_STAT_EXPIRED_TIME_CAP_REACHED_COUNT,"expired_time_cap_reached_count",NULL},
    {HDS_NOTE_STAT_EXPIRE_CYCLE_CPU_MILLISECONDS,"expire_cycle_cpu_milliseconds",NULL} ,
    {HDS_NOTE_STAT_EVICTED_KEYS,"evicted_keys",NULL}                ,  
    {HDS_NOTE_STAT_TOTAL_EVICTION_EXCEEDED_TIME,"total_eviction_exceeded_time",NULL},  
    {HDS_NOTE_STAT_CURRENT_EVICTION_EXCEEDED_TIME,"current_eviction_exceeded_time",NULL},
    {HDS_NOTE_STAT_KEYSPACE_HITS,"keyspace_hits",NULL}               ,  
    {HDS_NOTE_STAT_KEYSPACE_MISSES,"keyspace_misses",NULL}             ,  
    {HDS_NOTE_STAT_PUBSUB_CHANNELS,"pubsub_channels",NULL}             ,  
    {HDS_NOTE_STAT_PUBSUB_PATTERNS,"pubsub_patterns",NULL}             ,  
    {HDS_NOTE_STAT_LATEST_FORK_USEC,"latest_fork_usec",NULL}            ,  
    {HDS_NOTE_STAT_TOTAL_FORKS,"total_forks",NULL}                 ,  
    {HDS_NOTE_STAT_MIGRATE_CACHED_SOCKETS,"migrate_cached_sockets",NULL}      ,  
    {HDS_NOTE_STAT_SLAVE_EXPIRES_TRACKED_KEYS,"slave_expires_tracked_keys",NULL}  ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_HITS,"active_defrag_hits",NULL}          ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_MISSES,"active_defrag_misses",NULL}        ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_KEY_HITS,"active_defrag_key_hits",NULL}      ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_KEY_MISSES,"active_defrag_key_misses",NULL}    ,  
    {HDS_NOTE_STAT_TOTAL_ACTIVE_DEFRAG_TIME,"total_active_defrag_time",NULL}    ,  
    {HDS_NOTE_STAT_CURRENT_ACTIVE_DEFRAG_TIME,"current_active_defrag_time",NULL}  ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_KEYS,"tracking_total_keys",NULL}         ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_ITEMS,"tracking_total_items",NULL}        ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_PREFIXES,"tracking_total_prefixes",NULL}     ,  
    {HDS_NOTE_STAT_UNEXPECTED_ERROR_REPLIES,"unexpected_error_replies",NULL}    ,  
    {HDS_NOTE_STAT_TOTAL_ERROR_REPLIES,"total_error_replies",NULL}         ,  
    {HDS_NOTE_STAT_TOTAL_READS_PROCESSED,"total_reads_processed",NULL}       ,  
    {HDS_NOTE_STAT_TOTAL_WRITES_PROCESSED,"total_writes_processed",NULL}      ,  
    {HDS_NOTE_STAT_IO_THREADED_READS_PROCESSED,"io_threaded_reads_processed",NULL} ,  
    {HDS_NOTE_STAT_IO_THREADED_WRITES_PROCESSED,"io_threaded_writes_processed",NULL},
    {IDS_NOTE_PERSIST_LOADING,"loading",NULL},
    {IDS_NOTE_PERSIST_CURRENT_COW_PEAK,"current_cow_peak",NULL},
    {IDS_NOTE_PERSIST_CURRENT_COW_SIZE,"current_cow_size",NULL},
    {IDS_NOTE_PERSIST_CURRENT_FORK_PERC,"current_fork_perc",NULL},
    {IDS_NOTE_PERSIST_CURRENT_SAVE_KEYS_PROCESSED,"current_save_keys_processed",NULL},
    {IDS_NOTE_PERSIST_CURRENT_SAVE_KEYS_TOTAL,"current_save_keys_total",NULL},
    {IDS_NOTE_PERSIST_RDB_CHANGES_SINCE_LAST_SAVE,"rdb_changes_since_last_save",NULL},
    {IDS_NOTE_PERSIST_RDB_BGSAVE_IN_PROGRESS,"rdb_bgsave_in_progress",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_SAVE_TIME,"rdb_last_save_time",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_BGSAVE_STATUS,"rdb_last_bgsave_status",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_BGSAVE_TIME_SEC,"rdb_last_bgsave_time_sec",NULL},
    {IDS_NOTE_PERSIST_RDB_CURRENT_BGSAVE_TIME_SEC,"rdb_current_bgsave_time_sec",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_COW_SIZE,"rdb_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_AOF_ENABLED,"aof_enabled",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_IN_PROGRESS,"aof_rewrite_in_progress",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_SCHEDULED,"aof_rewrite_scheduled",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_REWRITE_TIME_SEC,"aof_last_rewrite_time_sec",NULL},
    {IDS_NOTE_PERSIST_AOF_CURRENT_REWRITE_TIME_SEC,"aof_current_rewrite_time_sec",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_BGREWRITE_STATUS,"aof_last_bgrewrite_status",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_WRITE_STATUS,"aof_last_write_status",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_COW_SIZE,"aof_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_MODULE_FORK_IN_PROGRESS,"module_fork_in_progress",NULL},
    {IDS_NOTE_PERSIST_MODULE_FORK_LAST_COW_SIZE,"module_fork_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_AOF_CURRENT_SIZE,"aof_current_size",NULL},
    {IDS_NOTE_PERSIST_AOF_BASE_SIZE,"aof_base_size",NULL},
    {IDS_NOTE_PERSIST_AOF_PENDING_REWRITE,"aof_pending_rewrite",NULL},
    {IDS_NOTE_PERSIST_AOF_BUFFER_LENGTH,"aof_buffer_length",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_BUFFER_LENGTH,"aof_rewrite_buffer_length",NULL},
    {IDS_NOTE_PERSIST_AOF_PENDING_BIO_FSYNC,"aof_pending_bio_fsync",NULL},
    {IDS_NOTE_PERSIST_AOF_DELAYED_FSYNC,"aof_delayed_fsync",NULL},
    {IDS_NOTE_PERSIST_LOADING_START_TIME,"loading_start_time",NULL},
    {IDS_NOTE_PERSIST_LOADING_TOTAL_BYTES,"loading_total_bytes",NULL},
    {IDS_NOTE_PERSIST_LOADING_RDB_USED_MEM,"loading_rdb_used_mem",NULL},
    {IDS_NOTE_PERSIST_LOADING_LOADED_BYTES,"loading_loaded_bytes",NULL},
    {IDS_NOTE_PERSIST_LOADING_LOADED_PERC,"loading_loaded_perc",NULL},
    {IDS_NOTE_PERSIST_LOADING_ETA_SECONDS,"loading_eta_seconds",NULL},
    {IDS_NOTE_REPLICAT_ROLE,"role",NULL},
    {IDS_NOTE_REPLICAT_MASTER_FAILOVER_STATE,"master_failover_state",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPLID,"master_replid",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPLID2,"master_replid2",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPL_OFFSET,"master_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_SECOND_REPL_OFFSET,"second_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_ACTIVE,"repl_backlog_active",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_SIZE,"repl_backlog_size",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_FIRST_BYTE_OFFSET,"repl_backlog_first_byte_offset",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_HISTLEN,"repl_backlog_histlen",NULL},
    {IDS_NOTE_REPLICAT_MASTER_HOST,"master_host",NULL},
    {IDS_NOTE_REPLICAT_MASTER_PORT,"master_port",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LINK_STATUS,"master_link_status",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LAST_IO_SECONDS_AGO,"master_last_io_seconds_ago",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_IN_PROGRESS,"master_sync_in_progress",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_REPL_OFFSET,"slave_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_PRIORITY,"slave_priority",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_READ_ONLY,"slave_read_only",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_TOTAL_BYTES,"master_sync_total_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_READ_BYTES,"master_sync_read_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_LEFT_BYTES,"master_sync_left_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_PERC,"master_sync_perc",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_LAST_IO_SECONDS_AGO,"master_sync_last_io_seconds_ago",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LINK_DOWN_SINCE_SECONDS,"master_link_down_since_seconds",NULL},
    {IDS_NOTE_REPLICAT_CONNECTED_SLAVES,"connected_slaves",NULL},
    {IDS_NOTE_REPLICAT_MIN_SLAVES_GOOD_SLAVES,"min_slaves_good_slaves",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS,"used_cpu_sys",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER,"used_cpu_user",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS_CHILDREN,"used_cpu_sys_children",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER_CHILDREN,"used_cpu_user_children",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS_MAIN_THREAD,"used_cpu_sys_main_thread",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER_MAIN_THREAD,"used_cpu_user_main_thread",NULL},
    {IDS_NOTE_CLUSTER_ENABLED,"cluster_enabled",NULL}
};

const int cids[3]={IDS_LV_COLUMN_PROPERTY,IDS_LV_COLUMN_VALUE,IDS_LV_COLUMN_NOTE};

char * find_message(const char * name){
    for(int ix = 0; ix < NOTE_COUNT; ix ++){
        if(strcmp(name,notes[ix].name) == 0){
            return notes[ix].message;
        }
    }
    return NULL;
}

void init_message(){
    for(int ix = 0; ix < NOTE_COUNT; ix ++){
       notes[ix].message = (char *)calloc(1,500);
       LoadString(App->hInstance,notes[ix].noteid,notes[ix].message,500);
    }
}


BOOL InitSetViewColumns1(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
        char * buff = (char *)calloc(1,255);
        LoadString(App->hInstance,cids[iCol],buff,255);
 
        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 200;
        lvc.fmt = LVCFMT_LEFT;

        ListView_InsertColumn(hWndListView, iCol, &lvc);
        //ListView_SetColumnWidth(hWndListView,iCol,(iCol+1) * 200);

        free(buff);
    }

    return TRUE;
}

HWND buildStatToolBar(HWND parent){
    int buttonCount = 11;
    TBBUTTON tbtn[11] = {
        {(TB_DELETE_BUTTON), TB_CMD_FLUSH_DB, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(1), IDM_STAT_SERVER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_STAT_CLIENT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_STAT_MEMORY, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_STAT_PERSISENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(5), IDM_STAT_STATS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_STAT_REPLICATION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(7), IDM_STAT_CPU, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_STAT_CLUSTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(9), IDM_STAT_KEYSPACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

BOOL updateConfigDataSet(HWND hwnd,KVPair kv){
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    for (int index = 0; index < kv->count; index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        lvI.pszText = kv->array[index]->key;
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = kv->array[index]->value;
        lvI.iSubItem = 1;
        //ListView_InsertItem(hwnd, &lvI);
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        char * msg = find_message(kv->array[index]->key);
        if(msg != NULL){
            lvI.pszText = msg;
            lvI.iSubItem = 2;
            SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
        }
    }

    return TRUE;
}

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    SystemViewModel model = (SystemViewModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (SystemViewModel)calloc(1,sizeof(struct system_view_model));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            GetClientRect (hwnd, &rect); 

            model->paramViewHwnd = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            model->toolBar = buildStatToolBar(hwnd);

            ListView_SetExtendedListViewStyle(model->paramViewHwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            InitSetViewColumns1(model->paramViewHwnd);

            init_message();
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch(msg->code){
                case TTN_GETDISPINFO:{
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                    lpttt->hinst = App->hInstance;
                    UINT_PTR idButton = lpttt->hdr.idFrom;
                    switch(idButton){
                        case TB_CMD_FLUSH_DB:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_FLUSH_ALL);break;}
                        case IDM_STAT_SERVER:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_SERVER);break;};
                        case IDM_STAT_CLIENT:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CLIENTS);break;}
                        case IDM_STAT_MEMORY:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_MEMORY);break;}
                        case IDM_STAT_PERSISENCE:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_PERSISTENCE);break;}
                        case IDM_STAT_STATS:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_STATS);break;}
                        case IDM_STAT_REPLICATION:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_REPLICATION);break;}
                        case IDM_STAT_CPU:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CPU);break;}
                        case IDM_STAT_CLUSTER:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CLUSTER);break;}
                        case IDM_STAT_KEYSPACE:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_KEYSPACE);break;}
                    }
                }
                break;
            }

            break;
        }
        case WM_COMMAND:{
            int cmd = LOWORD(wParam);
            if(cmd == TB_CMD_FLUSH_DB){
                int result = MessageBox(hwnd,"flushall will remove all data in all databases,continue?","wedis",MB_YESNOCANCEL|MB_ICONASTERISK);
                if(result == IDYES){
                    MessageBox(hwnd,"YES","title",MB_OK);
                }
            }else{
                stat_command(hwnd,LOWORD (wParam));
            }
            return 0;
        }

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            //TreeNode * datanode = (TreeNode *) lParam;

            KVPair kp = parseKVPair(data->bulk->content);

            updateConfigDataSet(model->paramViewHwnd,kp);
            break;
        }

        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            // MoveWindow(model->paramViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->paramViewHwnd,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

void stat_command(HWND hwnd,int cmd){
    const char * statcmd = "stats";
    switch (cmd){
        case IDM_STAT_SERVER:{statcmd = "server";break;}
        case IDM_STAT_CLIENT:{statcmd = "clients";break;}
        case IDM_STAT_MEMORY:{statcmd = "memory";break;}
        case IDM_STAT_PERSISENCE:{statcmd = "persistence";break;}
        case IDM_STAT_STATS:{statcmd = "stats";break;}
        case IDM_STAT_REPLICATION:{statcmd = "replication";break;}
        case IDM_STAT_CPU:{statcmd = "cpu";break;}
        case IDM_STAT_CLUSTER:{statcmd = "cluster";break;}
        case IDM_STAT_KEYSPACE:{statcmd = "keyspace";break;}
    }

    s_db_info_stats(App->activeHost,statcmd);
};

void init_systemview(HINSTANCE hInstance){
    WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.lpfnWndProc   = SystemViewWndProc;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hInstance     = hInstance;
    stringViewClass.hIcon         = NULL;
    stringViewClass.hCursor       = NULL;
    stringViewClass.hbrBackground = resource->brush; 
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = SYSTEM_VIEW_CLASS;
    stringViewClass.hIconSm       = NULL;
    RegisterClassEx(&stringViewClass);
}
