// -*- coding:utf-8 -*-
// Time-stamp: <2012-01-16 12:31 JST>
// confirm existance of the directory and execute

// this program will do
// - Wait seconds 指定秒のウェイト(-w 60)
// - Check existance of the directory ドライブ、フォルダの存在確認(-d "r:\cap")
// - execute 指定コマンド実行("C:\bin\foo.exe -a 1 -f \"r:\cap\aa.jpg\"")

// usage:
//   Windowsでconfexec.exeファイルへのショートカットを作り、プロパティのなかの
//  「ショートカット - リンク先」を以下にする
//     C:\home\bin\MiniCap\confexec.exe -w 3 -d "r:\cap" "MiniCap.exe -customdate \"$Y-$m-$d $H.$M\" -save \"r:\cap\$customdate$.jpg\" -capturedesktop -exit -resizexp 50 -resizeyp 50 -compress 3"
//   [ショートカットのプロパティ - ショートカット - 作業フォルダ」を以下にする
//     C:\home\bin\MiniCap

// how to compile (with gnupack-devel 7.02):
//    gcc -O2 -mwindows -o confexec.exe confexec.c ; strip confexec.exe


// func prototype
//
static int externalProcess(char *app, char *param, int wait);

//#include <stdio.h> // printf()
#include <stdlib.h> // exit(), system()
#include <string.h> // memset(), strncpy()
#include "getopt.h" //getopt()
#include <Windows.h> // Sleep()
#include <dirent.h> // opendir()
#include <sys/stat.h> // mkdir()
enum {WAIT, NOWAIT};


int main(int argc, char **argv){
    // variables for getopt
    //
    int opt;
    extern char *optarg;
    extern int optind, opterr;

    // variables
    //
    int wait_sec = 0;
    char dirname[256];
    memset(dirname, 0x00, 256);

    // command line option
    //
    while((opt = getopt(argc, argv, "w:d:")) != -1){
        switch(opt){
        case 'w':
            //printf("wait = %d\n", atoi(optarg));
            wait_sec = atoi(optarg);
            break;
        case 'd':
            //printf("dir = %s\n", optarg);
            strncpy(dirname, optarg, 255);
            break;
        default:
            break;
        }
    }
    argc -= optind;
    argv += optind;

    // confirm existance of argv[0]
    //
    if(argc < 1) exit(EXIT_FAILURE);
    //printf("error arg (%d) = %s\n", argc, argv[0]); exit(-1);

    // wait
    //
    Sleep(wait_sec * 1000);
    //printf("wait=%d dir=%s)\n", wait_sec, dirname);

    // confirm existance of a directory
    //
    if (*dirname != '\0'){
        DIR *dr;
        dr = opendir(dirname);
        if(dr == NULL){
            if(mkdir(dirname)) exit(EXIT_FAILURE);
        }
        // printf("%s is not found\n", dirname);
    }

    // execute
    //
    externalProcess(NULL, argv[0], NOWAIT);
    //system(argv[0]);  // <- CommandPrompt(MS-DOS prompt) appeared

    exit(EXIT_SUCCESS);
}

static int externalProcess(char *app, char *param, int wait){
    STARTUPINFO stinfo;
    PROCESS_INFORMATION psinfo;

    ZeroMemory( &stinfo, sizeof(stinfo) );
    stinfo.cb = sizeof(stinfo);
    ZeroMemory( &psinfo, sizeof(psinfo) );
    // if(!CreateProcess(app, param, NULL, NULL, FALSE, 0, NULL, 
    //                   NULL,
    //                   &stinfo, &psinfo)){
    //     //printf("error: CreateProcess(%s)\n", app);
    //     dnotice("%s の実行に失敗しました。", param);
    //     exit(1);
    // }
    CreateProcess(app, param, NULL, NULL, FALSE, 0, NULL, 
                  NULL,
                  &stinfo, &psinfo);
    // Wait until child process exits.
    if(wait == WAIT)
        WaitForSingleObject( psinfo.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( psinfo.hProcess );
    CloseHandle( psinfo.hThread );

    return 0;
}
