#ifndef DEFINE_H
#define DEFINE_H

#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <string.h>
#include <stdint.h>
using namespace std;



typedef unsigned char BYTE;
typedef unsigned char uint8_t;
//typedef unsigned long long uint64_t;


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


/*****************************************************************************
 * Error values (shouldn't be exposed)
 *****************************************************************************/
#define VLC_SUCCESS        (-0) /**< No error */
#define VLC_EGENERIC       (-1) /**< Unspecified error */
#define VLC_ENOMEM         (-2) /**< Not enough memory */
#define VLC_ETIMEOUT       (-3) /**< Timeout */
#define VLC_ENOMOD         (-4) /**< Module not found */
#define VLC_ENOOBJ         (-5) /**< Object not found */
#define VLC_ENOVAR         (-6) /**< Variable not found */
#define VLC_EBADVAR        (-7) /**< Bad variable value */
#define VLC_ENOITEM        (-8) /**< Item not found */


/* Branch prediction */
#ifdef __GNUC__
#   define likely(p)   __builtin_expect(!!(p), 1)
#   define unlikely(p) __builtin_expect(!!(p), 0)
#else
#   define likely(p)   (!!(p))
#   define unlikely(p) (!!(p))
#endif

typedef struct _INIT_PARAM_T
{
        _INIT_PARAM_T()
        {
                realtime_mode = false;
                continue_mode = false;
                single_file_mode = false;
                udp_port = 1234;
        }
        string url;
        string savepath;
        string savefile;
        string outname;
        string udp_send_addr;
        string udp_local_addr;
        int udp_port;
        bool realtime_mode;
        bool continue_mode;
        bool single_file_mode;
}INIT_PARAM_T;



/*****************************************************************************
 *
 *****************************************************************************/
#define AES_BLOCK_SIZE 16 /* Only support AES-128 */
typedef struct segment_s
{
        segment_s()
        {
                
        }
        segment_s(const segment_s &ss)
        {
                sequence        = ss.sequence;
                duration        = ss.duration;
                size            = ss.size;
                bandwidth       = ss.bandwidth;
                url             = ss.url;
                psz_key_path    = ss.psz_key_path;
                memcpy(aes_key,ss.aes_key,16);
                b_key_loaded    = ss.b_key_loaded;
        }
    int         sequence;   /* unique sequence number */
    int         duration;   /* segment duration (seconds) */
    uint64_t    size;       /* segment size in bytes */
    uint64_t    bandwidth;  /* bandwidth usage of segments (bits per second)*/

    string        url;
    string       psz_key_path;         /* url key path */
    uint8_t     aes_key[16];      /* AES-128 */
    bool        b_key_loaded;

    //add by elva
    //uint8_t      psz_AES_IV[AES_BLOCK_SIZE];    /* IV used when decypher the block */
} segment_t;



typedef struct stream_sys_s
{
        stream_sys_s()
        {
                b_live = false;
                b_error = false;
                b_meta = false;
                b_aesmsg = false;
        }
        string          m3u8;       /* M3U8 url */
        bool            b_live;     /* live stream? or vod? */
        bool            b_error;    /* parsing error */
        bool            b_meta;     /* meta playlist */
        bool            b_aesmsg;   /* only print one time that the media is encrypted */

}stream_sys_t;

typedef struct hls_stream_s
{
        hls_stream_s()
        {
                id = -1;
                version = -1;
                sequence = 0;
                duration = -1;
                bandwidth = 0;
                size = 0;
                segment_count = 0;
                
        }
        hls_stream_s(const hls_stream_s &hls)
        {
                id              = hls.id;
                version         = hls.version;
                sequence        = hls.sequence;
                duration        = hls.duration;
                bandwidth       = hls.bandwidth;
                size            = hls.size;
                url             = hls.url;
                b_cache         = hls.b_cache;
                psz_current_key_path = hls.psz_current_key_path;
                memcpy(psz_AES_IV,hls.psz_AES_IV,AES_BLOCK_SIZE);
                b_iv_loaded     = hls.b_iv_loaded;
                segment_count   = hls.segment_count;
        }
        
        int         id;         /* program id */
        int         version;    /* protocol version should be 1 */
        int         sequence;   /* media sequence number */
        int         duration;   /* maximum duration per segment (s) */
        uint64_t    bandwidth;  /* bandwidth usage of segments (bits per second)*/
        uint64_t    size;       /* stream length is calculated by taking the sum
                                foreach segment of (segment->duration * hls->bandwidth/8) */

        //vlc_array_t *segments;  /* list of segments */
        string        url;        /* uri to m3u8 */
        // vlc_mutex_t lock;
        bool        b_cache;    /* allow caching */

        string        psz_current_key_path;          /* URL path of the encrypted key */
        uint8_t      psz_AES_IV[AES_BLOCK_SIZE];    /* IV used when decypher the block */
        bool         b_iv_loaded;
        
        int          segment_count;
} hls_stream_t;


#endif

