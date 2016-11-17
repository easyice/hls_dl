#include "utils.h"
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>


int findprocess ( list<string>& arrayfile,string dirname )
{
        arrayfile.clear();
        int nCount=0;
        string dir ( dirname );
        struct stat statbuf;

        if ( stat ( dir.c_str(),&statbuf ) ==-1 )
        {
                printf ( "Get stat on %s Error:%s\n",dir.c_str(),strerror ( errno ) );
                return nCount;
        }

        if ( !S_ISDIR ( statbuf.st_mode ) )
        {
                printf ( "not dir :%s\n",dir.c_str() );
                return nCount;
        }

        DIR *dirp;
        struct dirent *direntp;
        if ( ( dirp=opendir ( dir.c_str() ) ) ==NULL )
        {
                printf ( "Open Directory %s Error:%s\n",dir.c_str(),strerror ( errno ) );
                return nCount;
        }
        while ( ( direntp=readdir ( dirp ) ) !=NULL )
        {
                string fname = direntp->d_name;
                if (fname == "." || fname == "..")
                {
                        continue;
                }

                string fullName ( dirname );
                fullName+="/";
                fullName+=fname;
                arrayfile.push_back ( fullName );
                
        }
        closedir ( dirp );

        return nCount;
}


string removeparam(const string& filename)
{
        if (filename.length() < 2)
        {
                return "";
        }
        int pos = 0;
        for (; pos < filename.length(); pos++)
        {
                if (filename[pos] == '?' ||
                        filename[pos] == '&')
                {
                        break;
                }
//                 if (!isalpha(filename[pos]))
//                 {
//                         break;
//                 }
        }
        if (pos == filename.length())
        {
                return filename;
        }

        if (pos <= 0)
        {
                return "";
        }
        return filename.substr(0,pos);
}


void CreateMulPath( const char *muldir )
{
        int    i,len;
        char    str[512];

        strncpy( str, muldir, 512 );
        len=strlen(str);
        //printf( "字符串%s长度=%d\n", str, len );
        for( i=0; i<len; i++ )
        {
                if( str[i]=='/' )
                {
                        str[i] = '\0';
                        //判断此目录是否存在,不存在则创建
                        if( access(str, F_OK)!=0 )
                        {
                                //printf( "创建第%d级目录 p=[%s]\n", i, str );
                                mkdir( str, 0777 );
                        }
                        str[i]='/';
                }
        }
        if( len>0 && access(str, F_OK)!=0 )
        {
                //printf( "最后一级目录=%s!!!!!!!\n", str );
                mkdir( str, 0777 );
        }
        return;
}


