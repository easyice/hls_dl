#include <stdio.h>
#include <string>


#include <getopt.h>
#include <unistd.h>
#include <vector>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "define.h"
#include "cdownloader.h"
#include <stdlib.h>



using namespace std;



void version()
{
        cout << "version 1.7" << endl;
        cout << "date:2015/1/6" << endl;
        _exit(0);
}

void usage(char *name)
{
        cout << "USAGE: "<< name <<" [OPTION]" << endl;
        cout << " -l set http link" << endl;
        cout << " -p set save path" << endl;
        cout << " -f set save filename" << endl;
        cout << " -o set output name" << endl;
        cout << " -u set udp dst: ip:port" << endl;
        cout << " -r realtime mode" << endl;
        cout << " -c continue mode" << endl;
        cout << " -h this help" << endl;
        _exit(0);
}


void scan_args(int argc,char **argv,INIT_PARAM_T& init_param)
{
        struct option longopts[]=
        {
                {"link",required_argument,NULL,'l'},
                {"savepath",required_argument,NULL,'p'},
                {"savefile",required_argument,NULL,'f'},
                {"outname",required_argument,NULL,'o'},
                {"udpdst",required_argument,NULL,'u'},
                {"udplocaladdr",required_argument,NULL,'a'},
                {"realtime",no_argument,NULL,'r'},
                {"continue",no_argument,NULL,'c'},
                {"help",no_argument,NULL,'h'},
                {"version",no_argument,NULL,'v'},
                {0,0,0,0},
        };
        
        int c;
        string str;
        while ( ( c = getopt_long_only ( argc,argv,"rchl:p:f:o:u:a:",longopts,NULL ) ) != -1 )
        {
                switch ( c )
                {
                        case 'l':
                                init_param.url = optarg;
                                break;
                        case 'p':
                                init_param.savepath = optarg;
                                break;
                        case 'f':
                                init_param.savefile = optarg;
                                break;
                        case 'o':
                                init_param.outname = optarg;
                                break;     
                        case 'u':
                                str = optarg;
                                init_param.udp_send_addr = str.substr(0,str.find(":"));
                                init_param.udp_port = atoi(str.substr(str.find(":")+1).c_str());
                                break; 
                        case 'a':
                                init_param.udp_local_addr = optarg;
                                break;         
                        case 'h':
                                usage(argv[0]);
                                break;
                        case 'r':
                                init_param.realtime_mode = true;
                                break;
                        case 'c':
                                init_param.continue_mode = true;
                                break;
                        case 'v':
                                version();
                                break;
                        default:
                                break;
                }
        }
}



int main(int argc,char **argv)
{
        signal(SIGPIPE, SIG_IGN);
        
        
        if (argc <= 1)
        {
                usage(argv[0]);
        }
        
        INIT_PARAM_T init_param;
        scan_args(argc,argv,init_param);
        if (!init_param.savepath.empty())
        {
                if (init_param.savepath[init_param.savepath.size()-1] != '/')
                {
                        init_param.savepath += "/";
                }
        }
        

//         cout << init_param.url << endl;
//         cout << init_param.realtime_mode << endl;
//         cout << init_param.savefile << endl;
//         cout << init_param.savepath << endl;
        
        CDownloader dl;
        dl.Run(init_param);
	return 0;
}
