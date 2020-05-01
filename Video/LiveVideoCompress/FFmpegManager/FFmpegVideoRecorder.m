//
//  FFmpegVideoRecorder.m
//  LiveVideoCompress
//
//  Created by vector on 2020/2/29.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "FFmpegVideoRecorder.h"

#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"

#import "FFVideoPixelBufferAdapter.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <Photos/Photos.h>


//官方
//http://www.ffmpeg.org/doxygen/4.0/examples.html


@interface FFAVStream ()

@end
@implementation FFAVStream

- (instancetype)initWithFormatContext:(FFAVFormatContext *)formatContext stream:(AVStream *)stream codec:(AVCodec *)codec {
    self = [super init];
    if (self) {
        _formatContext = formatContext;
        _stream = stream;
        _codec = codec;
    }
    return self;
}
@end


@interface FFAVFormatContext ()
@end
@implementation FFAVFormatContext

- (instancetype)initWithFormat:(FFAVFormat)format context:(AVFormatContext *)context path:(NSString *)path {
    self = [super init];
    if (self) {
        _format = format;
        _context = context;
        _path = [path copy];
    }
    return self;
}

+ (nullable instancetype)contextWithFormat:(FFAVFormat)format path:(NSString *)path {
    if (format != FFAVFormatMp4) {
        return nil;
    }
    AVOutputFormat *fmt = av_guess_format("MP4", NULL, NULL);
    AVFormatContext * formatContext = NULL;
    int code= avformat_alloc_output_context2(&formatContext, fmt, "MP4", path.UTF8String);
    if (code < 0) {
        return nil;
    }
    formatContext->oformat = fmt;
    return [[self alloc] initWithFormat:format context:formatContext path:path];
}

- (FFAVStream *)createStream:(AVCodec *)codec {
    if (codec) {
        AVStream *stream = avformat_new_stream(self.context, codec);
        if (stream) {
            return [[FFAVStream alloc] initWithFormatContext:self stream:stream codec:codec];
        } else {
            return nil;
        }
    } else {
        return nil;
    }
}

- (FFAVStream *)createAudioStream:(FFAudioCodec)codecType {
    if (FFAudioCodecAac == codecType) {
        return [self createStream:avcodec_find_encoder(AV_CODEC_ID_AAC)];
    } else {
        return nil;
    }
}
- (FFAVStream *)createVideoStream:(FFVideoCodec)codecType {
    if (FFVideoCodecH264 == codecType) {
        return [self createStream:avcodec_find_encoder(AV_CODEC_ID_H264)];
    } else {
        return nil;
    }
}

@end



@interface FFBaseAVAdapter ()

@end
@implementation FFBaseAVAdapter

- (instancetype)initWithFormatContext:(FFAVFormatContext *)formatContext stream:(FFAVStream *)stream context:(AVCodecContext *)context {
    self = [super init];
    if (self) {
        _formatContext = formatContext;
        _stream = stream;
        _context = context;
    }
    return self;
}
- (BOOL)close {
    return avcodec_close(self.context);
}


@end


@implementation FFAudioOption

- (instancetype)init {
    self = [super init];
    if (self) {
        _codec = FFAudioCodecAac;
    }
    return self;
}
@end
@implementation FFVideoOption

- (instancetype)init {
    self = [super init];
    if (self) {
        _width = 1280;
        _height = 720;
        _codec = FFVideoCodecH264;
        _frameRate = 30;
    }
    return self;
}

@end



@interface FFAudioAdapter ()

@end
@implementation FFAudioAdapter



@end

@interface FFVideoAdapter ()

@property (nonatomic, assign) uint32_t width;
@property (nonatomic, assign) uint32_t height;

@end
@implementation FFVideoAdapter

//
///*
// */
//
//- (AVFrame *)makeFrame:(enum AVPixelFormat)fmt width:(int)width height:(int)height {
//    AVFrame * picture = av_frame_alloc();
//    uint8_t * picture_buf = NULL;
//
//    if (!picture) {
//        return NULL;
//    }
//
//    int size = av_image_get_buffer_size(fmt, width, height, 64);
//    if (size < 0) {
//        return NULL;
//    }
//    int line = 0;
//    int createImageBufferResult = av_image_alloc(&picture_buf, &line, width, height, fmt, 64);
//    if (!picture_buf) {
//        av_frame_free(&picture);
//        return NULL;
//    }
//    avpicture_fill((AVPicture *)picture, picture_buf,
//                   pix_fmt, width, height);
//    return picture;
//}
//
//- (void)writeFrame:(id)frame {
//    int out_size, ret;
//    AVFormatContext * formatContext = self.formatContext.formatContext;
//    AVCodecContext * context = self.context;
//    AVStream * stream = self.stream.stream;
//
////    static struct SwsContext *img_convert_ctx;
////    if (context->pix_fmt != AV_PIX_FMT_YUV420P) {
////        /* as we only generate a YUV420P picture, we must convert it
////           to the codec pixel format if needed */
////        if (img_convert_ctx == NULL) {
////            img_convert_ctx = sws_getContext(c->width, c->height,
////                                             PIX_FMT_YUV420P,
////                                             c->width, c->height,
////                                             c->pix_fmt,
////                                             sws_flags, NULL, NULL, NULL);
////            if (img_convert_ctx == NULL) {
////                fprintf(stderr, "Cannot initialize the conversion context\n");
////                exit(1);
////            }
////        }
////        fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
////        sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
////                  0, c->height, picture->data, picture->linesize);
////    } else {
////        fill_yuv_image(picture, frame_count, c->width, c->height);
////    }
//
//    if (formatContext->oformat->flags & AVFMT_NOFILE) {
//        /* raw video case. The API will change slightly in the near
//           futur for that */
//        AVPacket pkt;
//        av_init_packet(&pkt);
//
//        pkt.flags |= AV_PKT_FLAG_KEY;
//        pkt.stream_index= stream->index;
//        pkt.data= (uint8_t *)picture;
//        pkt.size = sizeof(AVPicture);
//
//        ret = av_interleaved_write_frame(formatContext, &pkt);
//    } else {
//        /* encode the image */
//
//        avcodec_send_frame(<#AVCodecContext *avctx#>, <#const AVFrame *frame#>)
//
////        avcodec_receive_packet(AVCodecContext *avctx, <#AVPacket *avpkt#>)
////
////        avcodec_send_frame(context, &)
//
////        avcodec_send_frame()/avcodec_receive_packet() instead
//
//        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
//        /* if zero size, it means the image was buffered */
//        if (out_size > 0) {
//            AVPacket pkt;
//            av_init_packet(&pkt);
//
//            if (c->coded_frame->pts != AV_NOPTS_VALUE)
//                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
//            if(c->coded_frame->key_frame)
//                pkt.flags |= PKT_FLAG_KEY;
//            pkt.stream_index= st->index;
//            pkt.data= video_outbuf;
//            pkt.size= out_size;
//
//            /* write the compressed frame in the media file */
//            ret = av_interleaved_write_frame(oc, &pkt);
//        } else {
//            ret = 0;
//        }
//    }
//    if (ret != 0) {
//        fprintf(stderr, "Error while writing video frame\n");
//        exit(1);
//    }
//    frame_count++;
//}
//


@end






@interface FFAVWriter ()


@property (nonatomic, assign) AVStream * audioStream;
@property (nonatomic, assign) AVStream * videoStream;

@property (nonatomic, assign) int16_t * samples;
@property (nonatomic, assign) uint8_t * audio_outbuf;

@property (nonatomic, assign) int audio_outbuf_size;
@property (nonatomic, assign) int audio_input_frame_size;

//float t, tincr, tincr2;

@property (nonatomic, assign) int videoFrameRate;

@property (nonatomic, assign) AVCodecContext * videoCodecContext;
@property (nonatomic, assign) AVCodecContext * audioCodecContext;

@property (nonatomic, strong, nullable) FFAudioAdapter * audioAdapter;
@property (nonatomic, strong, nullable) FFAudioAdapter * videoAdapter;

@end
@implementation FFAVWriter

- (instancetype)initWithContext:(FFAVFormatContext *)context {
    self = [super init];
    if (self) {
        _context = context;
    }
    return self;
}

//- (instancetype)initWithFormatContext:(AVFormatContext *)formatContext {
//    self = [super init];
//    if (self) {
//        _formatContext = formatContext;
//    }
//    return self;
//}
//
//+ (nullable instancetype)contextWithFormat:(FFAVFormat)format {
//    if (format != FFAVFormatMp4) {
//        return nil;
//    }
//    AVOutputFormat *fmt = av_guess_format("MP4", NULL, NULL);
//    AVFormatContext * formatContext = NULL;
//    int code= avformat_alloc_output_context2(&formatContext, fmt, "MP4", NULL);
//    if (code < 0) {
//        return nil;
//    }
//    formatContext->oformat = fmt;
//    return [[self alloc] initWithFormatContext:formatContext];
//}
//
//- (FFAVStream *)createStream:(AVCodec *)codec {
//    if (codec) {
//        AVStream *stream = avformat_new_stream(self.formatContext, codec);
//        if (stream) {
//            return [[FFAVStream alloc] initWithFormatContext:self stream:stream codec:codec];
//        } else {
//            return nil;
//        }
//    } else {
//        return nil;
//    }
//}
//
//- (FFAVStream *)createAudioStream:(FFAudioCodec)codecType {
//    if (FFAudioCodecAac == codecType) {
//        return [self createStream:avcodec_find_encoder(AV_CODEC_ID_AAC)];
//    } else {
//        return nil;
//    }
//}
//- (FFAVStream *)createVideoStream:(FFVideoCodec)codecType {
//    if (FFVideoCodecH264 == codecType) {
//        return [self createStream:avcodec_find_encoder(AV_CODEC_ID_H264)];
//    } else {
//        return nil;
//    }
//}


+ (nullable instancetype)writerWithFormat:(FFAVFormat)format path:(NSString *)path {
    if (path.length <= 0) {
        return nil;
    }
    FFAVFormatContext * context = [FFAVFormatContext contextWithFormat:format path:path];
    if (nil == context) {
        return nil;
    }
    return [[FFAVWriter alloc] initWithContext:context];
}

- (nullable FFAudioAdapter *)addAudioAdapter:(FFAudioOption *)option {
    FFAVStream * stream = [self.context createAudioStream:option.codec];
    AVCodecContext * context = avcodec_alloc_context3(stream.codec);
    context->codec_id = stream.codec->id;
    context->codec_type = AVMEDIA_TYPE_AUDIO;

    /* put sample parameters */
    context->bit_rate = 64000;
    context->sample_rate = 44100;
    context->channels = 2;
    context->sample_fmt = AV_SAMPLE_FMT_FLTP;
    context->channel_layout = av_get_default_channel_layout(context->channels);

    /* open it */
    int openResult = avcodec_open2(context, stream.codec, NULL);
    if (0 != openResult) {
        fprintf(stderr, "could not open codec\n");
        abort();
    }
    int mapResult = avcodec_parameters_from_context(stream.stream->codecpar, context);
    if (mapResult < 0) {
        fprintf(stderr, "could not avcodec_parameters_from_context\n");
        abort();
    }

    return [[FFAudioAdapter alloc] initWithFormatContext:self.context stream:stream context:context];
    
    //    /* init signal generator */
    //    t = 0;
    //    tincr = 2 * M_PI * 110.0 / params->sample_rate;
    //    /* increment frequency by 110 Hz per second */
    //    tincr2 = 2 * M_PI * 110.0 / params->sample_rate / params->sample_rate;
    //
    //    audio_outbuf_size = 10000;
    //    audio_outbuf = av_malloc(audio_outbuf_size);
    //
    //    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
    //       support to compute the input frame size in samples */
    //    if (params->frame_size <= 1) {
    //        audio_input_frame_size = audio_outbuf_size / params->channels;
    //        switch(st->codecpar->codec_id) {
    //        case AV_CODEC_ID_PCM_S16LE:
    //        case AV_CODEC_ID_PCM_S16BE:
    //        case AV_CODEC_ID_PCM_U16LE:
    //        case AV_CODEC_ID_PCM_U16BE:
    //            audio_input_frame_size >>= 1;
    //            break;
    //        default:
    //            break;
    //        }
    //    } else {
    //        audio_input_frame_size = params->frame_size;
    //    }
    //    samples = av_malloc(audio_input_frame_size * 2 * params->channels);
    
}

/*
 i:0, AVPixelFormat 0 AV_PIX_FMT_YUV420P
 i:1, AVPixelFormat 12 AV_PIX_FMT_YUVJ420P
 i:2, AVPixelFormat 4 AV_PIX_FMT_YUV422P
 i:3, AVPixelFormat 13 AV_PIX_FMT_YUVJ422P
 i:4, AVPixelFormat 5 AV_PIX_FMT_YUV444P
 i:5, AVPixelFormat 14 AV_PIX_FMT_YUVJ444P
 i:6, AVPixelFormat 23 AV_PIX_FMT_NV12
 i:7, AVPixelFormat 103 AV_PIX_FMT_NV16
 i:8, AVPixelFormat 24 AV_PIX_FMT_NV21
 i:9, AVPixelFormat 64 AV_PIX_FMT_YUV420P10LE
 i:10, AVPixelFormat 66 AV_PIX_FMT_YUV422P10LE
 i:11, AVPixelFormat 70 AV_PIX_FMT_GRAY10LE
 i:12, AVPixelFormat 104 AV_PIX_FMT_ARGB
 i:13, AVPixelFormat 8 AV_PIX_FMT_GRAY8
 i:14, AVPixelFormat 171 AV_PIX_FMT_GRAY10LE
 */
- (nullable FFVideoAdapter *)addVideoAdapter2:(FFVideoOption *)option {
    if (FFVideoCodecH264 != option.codec) {
        return nil;
    }
    AVFormatContext * fContext = self.context.context;
    
    AVCodec * codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (NULL == codec) {
        return nil;
    }
    AVCodecContext * context = avcodec_alloc_context3(codec);
    
    context->flags |= AV_CODEC_FLAG_QSCALE;
    if (fContext->oformat->flags & AVFMT_GLOBALHEADER) {
        context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->width = option.width;
    context->height = option.height;
    context->pix_fmt = AV_PIX_FMT_NV12;
    context->gop_size = option.frameRate * 3;
    context->max_b_frames = 1;
    context->thread_count = 1;
    context->ticks_per_frame = 2;
    AVRational timeBase = {.num = 1, .den = option.frameRate};

    context->time_base = timeBase;
    context->pkt_timebase = timeBase;
    context->qmin = 10;
    context->qmax = 51;
    context->qcompress  = 0.6;
    
    AVRational framerate = {.num = option.frameRate, .den = 1};
    context->framerate = framerate;//帧率 = num/den
    context->bit_rate = option.width * option.height * option.frameRate / 10 * 0.7;//362880
//    1400 * 1000;
    context->rc_max_rate = option.width * option.height * option.frameRate / 10 * 0.7;
    context->rc_min_rate = option.width * option.height * option.frameRate / 10 * 0.7 / 2;    
    
    int openResult = avcodec_open2(context, codec, NULL);
    if (0 != openResult) {
        fprintf(stderr, "could not open codec\n");
        abort();
    }
    
    FFAVStream * stream = [self.context createVideoStream:option.codec];
    stream.stream->id = 0;
    stream.stream->codecpar->codec_tag = 0;
    stream.stream->time_base = context->time_base;
//stream.stream->fr
    
    int mapResult = avcodec_parameters_from_context(stream.stream->codecpar, context);

    av_dump_format(self.context.context, 0, self.context.path.UTF8String, 1);

    if (mapResult < 0) {
        fprintf(stderr, "could not avcodec_parameters_from_context\n");
        abort();
    }
    FFVideoAdapter * adapter = [[FFVideoAdapter alloc] initWithFormatContext:self.context stream:stream context:context];
    adapter.width = option.width;
    adapter.height = option.height;
    return adapter;
}


- (nullable FFVideoAdapter *)addVideoAdapter:(FFVideoOption *)option {
    FFAVStream * stream = [self.context createVideoStream:option.codec];
    
    AVCodec * codec = stream.codec;
    
    
//    enum AVPixelFormat F171 = 171;
//    enum AVPixelFormat F70 = 70;
//    enum AVPixelFormat F66 = 66;
//    enum AVPixelFormat F64 = 64;
    /*
     
     YUV 4:4:4采样，每一个Y对应一组UV分量8+8+8 = 24bits,3个字节。
     YUV 4:2:2采样，每两个Y共用一组UV分量,一个YUV占8+4+4 = 16bits 2个字节。
     YUV 4:2:0采样，每四个Y共用一组UV分量一个YUV占8+2+2 = 12bits 1.5个字节。
     所以YUV采样方式主要有：YUV444,YUV422,YUV420，三种YUV采样模式的表示

     YUV444
     （1）YUV444p：YYYYYYYYY VVVVVVVVV UUUUUUUU
     YUV422
     （1）YUV422p：YYYYYYYY VVVV UUUU
     （2）YUVY：YUYV YUYV YUYV YUYV
     （3）UYVY：UYVY UYVY UYVY UYVY
     YUV420
     （1）YUV420p：
     YV12：YYYYYYYY VV UU
     YU12（I420）：YYYYYYYY UU VV
     (2）YUV420sp：
     NV12：YYYYYYYY UVUV
     NV21：YYYYYYYY VUVU
     
     H264硬编码 YUV420SP: {
     AV_PIX_FMT_NV12,
     AV_PIX_FMT_NV21,
     }
     NV12:IOS只有这一种模式。存储顺序是先存Y，再UV交替存储。YYYYUVUVUV
     NV21:安卓的模式。存储顺序是先存Y，再存U，再VU交替存储。YYYYVUVUVU
     */

    AVCodecParameters * params = stream.stream->codecpar;
    params->codec_id = stream.codec->id;

    AVCodecContext * context = avcodec_alloc_context3(stream.codec);
    context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->width = option.width;
    context->height = option.height;
    context->pix_fmt = AV_PIX_FMT_NV12;
    context->gop_size = 10;
    context->max_b_frames = 1;
    context->time_base.num = 1;
    context->thread_count = 2;
    context->time_base.den = option.frameRate;
    context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    context->qmin = 10;
    context->qmax = 51;
    context->qcompress  = 0.6;
    
    AVRational framerate = {.num = option.frameRate, .den = 1};
    context->framerate = framerate;
    context->bit_rate = 1400 * 1000;
    
    int openResult = avcodec_open2(context, stream.codec, NULL);
    if (0 != openResult) {
        fprintf(stderr, "could not open codec\n");
        abort();
    }

    if (codec->id == AV_CODEC_ID_H264) {
           
        av_opt_set(context->priv_data, "preset", "slow", 0);
    }
    int mapResult = avcodec_parameters_from_context(stream.stream->codecpar, context);

    av_dump_format(self.context.context, 0, self.context.path.UTF8String, 1);

    if (mapResult < 0) {
        fprintf(stderr, "could not avcodec_parameters_from_context\n");
        abort();
    }
    FFVideoAdapter * adapter = [[FFVideoAdapter alloc] initWithFormatContext:self.context stream:stream context:context];
    adapter.width = option.width;
    adapter.height = option.height;
    return adapter;
}



/*
 * add an audio output stream
 */
- (AVStream *)addAudioStream:(AVFormatContext *) oc codecId:(enum AVCodecID)codec_id {
    AVStream *st;
    AVCodecParameters * params;

//    AVCodec
        
    AVCodec *codec = avcodec_find_encoder(codec_id);
    st = avformat_new_stream(oc, codec);
    
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }

    params = st->codecpar;
    params->codec_id = codec_id;
    params->codec_type = AVMEDIA_TYPE_AUDIO;

    /* put sample parameters */
    params->bit_rate = 64000;
    params->sample_rate = 44100;
    params->channels = 2;
    return st;
}

- (AVCodecContext *)openAudio:(AVFormatContext *)oc stream:(AVStream *)st {
    AVCodecParameters * params;
    AVCodec *codec;

    params = st->codecpar;

    /* find the audio encoder */
    codec = avcodec_find_encoder(params->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        abort();
    }

    
    AVCodecContext * ctx = avcodec_alloc_context3(codec);
    int result = avcodec_parameters_to_context(ctx, params);
    if (result < 0) {
        fprintf(stderr, "avcodec_parameters_to_context error\n");
        abort();
    }

    /* open it */
    int openResult = avcodec_open2(ctx, codec, NULL);
    if (openResult) {
        fprintf(stderr, "could not open codec\n");
        abort();
    }

    
    return ctx;
    
    
    
    
//    /* init signal generator */
//    t = 0;
//    tincr = 2 * M_PI * 110.0 / params->sample_rate;
//    /* increment frequency by 110 Hz per second */
//    tincr2 = 2 * M_PI * 110.0 / params->sample_rate / params->sample_rate;
//
//    audio_outbuf_size = 10000;
//    audio_outbuf = av_malloc(audio_outbuf_size);
//
//    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
//       support to compute the input frame size in samples */
//    if (params->frame_size <= 1) {
//        audio_input_frame_size = audio_outbuf_size / params->channels;
//        switch(st->codecpar->codec_id) {
//        case AV_CODEC_ID_PCM_S16LE:
//        case AV_CODEC_ID_PCM_S16BE:
//        case AV_CODEC_ID_PCM_U16LE:
//        case AV_CODEC_ID_PCM_U16BE:
//            audio_input_frame_size >>= 1;
//            break;
//        default:
//            break;
//        }
//    } else {
//        audio_input_frame_size = params->frame_size;
//    }
//    samples = av_malloc(audio_input_frame_size * 2 * params->channels);
}


//AVFrame *picture, *tmp_picture;
//uint8_t *video_outbuf;
//int frame_count, video_outbuf_size;

/* add a video output stream */
- (AVStream *)addVideoStream:(AVFormatContext *)formatContext codecId:(enum AVCodecID)codec_id {
    AVCodecParameters * params;
    AVStream * stream;
    
    AVCodec *codec = avcodec_find_encoder(codec_id);
    stream = avformat_new_stream(formatContext, codec);
    
    if (!stream) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }

    params = stream->codecpar;
    params->codec_id = codec_id;
    params->codec_type = AVMEDIA_TYPE_VIDEO;

    params->bit_rate = 400000;
    //分辨率可能会有些别的限制， 比如 必须是x的倍数
    params->width = 352;
    params->height = 288;

    params->format = AV_PIX_FMT_ARGB;
    params->sample_aspect_ratio.den = self.videoFrameRate;
    params->sample_aspect_ratio.num = 1;

    stream->time_base.den = self.videoFrameRate;
    stream->time_base.num = 1;

//    AVCodecContext *c;
//
//    c->pix_fmt;
//
//    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
//    c->pix_fmt = STREAM_PIX_FMT;
//    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
//        /* just for testing, we also add B frames */
//        c->max_b_frames = 2;
//    }
//    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
//        /* Needed to avoid using macroblocks in which some coeffs overflow.
//           This does not happen with normal video, it just happens here as
//           the motion of the chroma plane does not match the luma plane. */
//        c->mb_decision=2;
//    }
//    // some formats want stream headers to be separate
//    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
//        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return stream;
}

//static AVFrame *alloc_picture(int pix_fmt, int width, int height)
//{
//    AVFrame *picture;
//    uint8_t *picture_buf;
//    int size;
//
//    picture = avcodec_alloc_frame();
//    if (!picture)
//        return NULL;
//    size = avpicture_get_size(pix_fmt, width, height);
//    picture_buf = av_malloc(size);
//    if (!picture_buf) {
//        av_free(picture);
//        return NULL;
//    }
//    avpicture_fill((AVPicture *)picture, picture_buf,
//                   pix_fmt, width, height);
//    return picture;
//}
//
//
//- (void)doVideo {
//    AVFormatContext * formatContext = self.formatContext;
//    /* find the video encoder */
//    AVCodec * codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//    AVCodecContext * context = avcodec_alloc_context3(codec);
//
////    //new一个流并挂到fmt_ctx名下，调用avformat_free_context时会释放该流
//    AVStream *stream = avformat_new_stream(formatContext, codec);
//
//    context->codec_type = AVMEDIA_TYPE_VIDEO;
//    context->codec_id = AV_CODEC_ID_H264;
//    context->width = 1280;
//    context->height = 720;
//    context->pix_fmt = AV_PIX_FMT_YUV420P;
//    context->gop_size = 12;
//    context->time_base.den = self.videoFrameRate;
//    context->time_base.num = 1;
//    context->bit_rate = 1400 * 1000;
//
//    int openResult = avcodec_open2(context, codec, NULL);
//    int mapResult = avcodec_parameters_from_context(stream->codecpar, context);
//
////    codec_ctx->width = 1280;//你想要的宽度
////    codec_ctx->height = 720;//你想要的高度
////    codec_ctx->format = AV_PIX_FMT_YUV420P;//受codec->pix_fmts数组限制
////
////    codec_ctx->gop_size = 12;
////
////    codec_ctx->time_base = AVRational{1, 25};//应该根据帧率设置
////    codec_ctx->bit_rate = 1400 * 1000;
////
////    avcodec_open2(codec_ctx, codec, nullptr);
////    //将AVCodecContext的成员复制到AVCodecParameters结构体。前后两行不能调换顺序
////    avcodec_parameters_from_context(stream->codecpar, codec_ctx);
////
////    av_stream_set_r_frame_rate(stream, {1, 25});
//
//
//}

/*
 
 https://blog.csdn.net/luotuo44/article/details/54981809
 const char *filename = "b.mp4";
 AVFormatContext *fmt_ctx = nullptr;
 avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, filename); //需要调用avformat_free_context释放

 //new一个流并挂到fmt_ctx名下，调用avformat_free_context时会释放该流
 AVStream *stream = avformat_new_stream(fmt_ctx, nullptr);
 AVCodec *codec = avcodec_find_encoder(fmt_ctx->oformat->video_codec);//音频为audio_codec
 AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);

 codec_ctx->video_type = AVMEDIA_TYPE_VIDEO;
 codec_ctx->codec_id = m_fmt_ctx->oformat->video_codec;

 codec_ctx->width = 1280;//你想要的宽度
 codec_ctx->height = 720;//你想要的高度
 codec_ctx->format = AV_PIX_FMT_YUV420P;//受codec->pix_fmts数组限制

 codec_ctx->gop_size = 12;

 codec_ctx->time_base = AVRational{1, 25};//应该根据帧率设置
 codec_ctx->bit_rate = 1400 * 1000;

 avcodec_open2(codec_ctx, codec, nullptr);
 //将AVCodecContext的成员复制到AVCodecParameters结构体。前后两行不能调换顺序
 avcodec_parameters_from_context(stream->codecpar, codec_ctx);

 av_stream_set_r_frame_rate(stream, {1, 25});

 */

- (AVCodecContext *)openVideo:(AVFormatContext *)formatContext stream:(AVStream *)stream {
    AVCodec *codec;
    AVCodecParameters * params;

    params = stream->codecpar;
    /* find the video encoder */
    codec = avcodec_find_encoder(params->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        abort();
    }
    
    AVCodecContext * ctx = avcodec_alloc_context3(codec);
    ctx->time_base.den = self.videoFrameRate;
    ctx->gop_size = 12;/* emit one intra frame every twelve frames at most */

    /* open the codec */
    int openResult = avcodec_open2(ctx, codec, NULL);
    if (openResult) {
        fprintf(stderr, "could not open codec\n");
        abort();
    }
    
    int result = avcodec_parameters_to_context(ctx, params);
    if (result < 0) {
        fprintf(stderr, "avcodec_parameters_to_context error\n");
        abort();
    }

    return ctx;


//    video_outbuf = NULL;
//    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
//        /* allocate output buffer */
//        /* XXX: API change will be done */
//        /* buffers passed into lav* can be allocated any way you prefer,
//           as long as they're aligned enough for the architecture, and
//           they're freed appropriately (such as using av_free for buffers
//           allocated with av_malloc) */
//        video_outbuf_size = 200000;
//        video_outbuf = av_malloc(video_outbuf_size);
//    }
//
//    /* allocate the encoded raw picture */
//    picture = alloc_picture(c->pix_fmt, c->width, c->height);
//    if (!picture) {
//        fprintf(stderr, "Could not allocate picture\n");
//        exit(1);
//    }
//
//    /* if the output format is not YUV420P, then a temporary YUV420P
//       picture is needed too. It is then converted to the required
//       output format */
//    tmp_picture = NULL;
//    if (c->pix_fmt != PIX_FMT_YUV420P) {
//        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
//        if (!tmp_picture) {
//            fprintf(stderr, "Could not allocate temporary picture\n");
//            exit(1);
//        }
//    }
}

- (void)open {
    AVFormatContext * formatContext = self.context.context;
    if (!(formatContext->flags & AVFMT_NOFILE)) {
        int oResult = avio_open2(&(formatContext->pb), self.context.path.UTF8String, AVIO_FLAG_READ_WRITE, NULL, NULL);
        if (oResult < 0) {
            NSLog(@"Could not open %@", self.context.path);
        } else {
            NSLog(@"avio_open2 success");
        }
    } else {
        NSLog(@"AVFMT_NOFILE eee");
    }
    
    int writeHeaderResult = avformat_write_header(formatContext, NULL);
    if (writeHeaderResult == AVSTREAM_INIT_IN_WRITE_HEADER) {
        NSLog(@"avformat_write_header success AVSTREAM_INIT_IN_WRITE_HEADER");
    } else if (writeHeaderResult == AVSTREAM_INIT_IN_INIT_OUTPUT) {
        NSLog(@"avformat_write_header success AVSTREAM_INIT_IN_INIT_OUTPUT");
    } else {
        NSLog(@"writeHeaderResult %d", writeHeaderResult);
    }
    
}

- (void)close {
    AVFormatContext * formatContext = self.context.context;

    int r = av_write_trailer(formatContext);
    if (r != 0) {
        NSLog(@"av_write_trailer failure %ld", r);
    } else {
        NSLog(@"av_write_trailer success");
    }
    int closeResult = avio_closep(&(formatContext->pb));
    if (closeResult < 0) {
        NSLog(@"avio_closep failure %ld", closeResult);
    } else {
        NSLog(@"avio_closep success %ld", closeResult);
    }
}

+ (void)test {
    NSTimeInterval beginTime = [NSDate date].timeIntervalSince1970;
    NSString * path = [NSHomeDirectory() stringByAppendingFormat:@"/Documents/ff_%@.mp4", [NSUUID UUID].UUIDString];
    NSLog(@"path: %@", path);
    FFAVWriter * writer = [FFAVWriter writerWithFormat:FFAVFormatMp4 path:path];
    
    FFAudioOption * audio = [[FFAudioOption alloc] init];
    audio.codec = FFAudioCodecAac;
    
    FFVideoOption * video = [[FFVideoOption alloc] init];
    video.width = 960;
    video.height = 540;
    video.frameRate = 15;
    video.codec = FFVideoCodecH264;

//    [writer addAudioAdapter:audio];
    
    FFVideoAdapter * v = [writer addVideoAdapter2:video];
    
    
    FFVideoPixelBufferAdapter * ada = [[FFVideoPixelBufferAdapter alloc] initWithVideoAdapter:v];
    
    [writer open];
    //
    //    for(;;) {
    //        /* compute current audio and video time */
    //        if (audio_st)
    //            audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
    //        else
    //            audio_pts = 0.0;
    //
    //        if (video_st)
    //            video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
    //        else
    //            video_pts = 0.0;
    //
    //        if ((!audio_st || audio_pts >= STREAM_DURATION) &&
    //            (!video_st || video_pts >= STREAM_DURATION))
    //            break;
    //
    //        /* write interleaved audio and video frames */
    //        if (!video_st || (video_st && audio_st && audio_pts < video_pts)) {
    //            write_audio_frame(oc, audio_st);
    //        } else {
    //            write_video_frame(oc, video_st);
    //        }
    //    }
    //
    //    /* write the trailer, if any.  the trailer must be written
    //     * before you close the CodecContexts open when you wrote the
    //     * header; otherwise write_trailer may try to use memory that
    //     * was freed on av_codec_close() */
    //    av_write_trailer(oc);
    //
    //    /* close each codec */
    //    if (video_st)
    //        close_video(oc, video_st);
    //    if (audio_st)
    //        close_audio(oc, audio_st);
    //
    //    /* free the streams */
    //    for(i = 0; i < oc->nb_streams; i++) {
    //        av_freep(&oc->streams[i]->codec);
    //        av_freep(&oc->streams[i]);
    //    }
    //
    //    if (!(fmt->flags & AVFMT_NOFILE)) {
    //        /* close the output file */
    //        url_fclose(oc->pb);
    //    }
    //
    //    /* free the stream */
    //    av_free(oc);
    
    UIImage * image = [UIImage imageNamed:@"984916-20160701214405843-875974577.jpg"];
    
    
    double d = 1000.0 / 10;
        
    for (int i=0; i<= 150; i++) {
        double time = 1.0 / 15 * i;
        NSLog(@"time: %.03lf", time);

        [ada writeFrameAtTime:time handler:^(CGContextRef  _Nonnull context) {
            if (i % 30 == 0) {
                CGContextSetFillColorWithColor(context, UIColor.whiteColor.CGColor);
                CGContextFillRect(context, CGRectMake(0, 0, video.width, video.height));
            }
            
            CGContextDrawImage(context, CGRectMake(arc4random() % 400, arc4random() % 300, 400, 600), image.CGImage);
        }];
    }
    [ada finish];
    [writer close];
    
    NSDictionary<NSFileAttributeKey, id> * info = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:NULL];
    NSLog(@"file size %@", info[NSFileSize]);
    
    AVAsset * asset = [AVAsset assetWithURL:[NSURL fileURLWithPath:path]];
    NSLog(@"duration:  %.03lf", CMTimeGetSeconds(asset.duration));
    
    NSTimeInterval endTime = [NSDate date].timeIntervalSince1970;
    NSLog(@"used:  %.03lf", endTime - beginTime);

    BOOL success = YES;
    PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatus];
    if (status == PHAuthorizationStatusNotDetermined) {
        //进行询问
        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status) {
            if (status == PHAuthorizationStatusAuthorized) {
                [writer saveVideo:path];
            } else {
                
            }
        }];
        return;
    } else {
        if (status == PHAuthorizationStatusRestricted || status == PHAuthorizationStatusDenied) {

        }
        if (status == PHAuthorizationStatusAuthorized) {
            
            [writer saveVideo:path];

        }
    }
    
    

}


- (PHAssetCollectionChangeRequest *)getCurrentPhotoCollectionWithAlbumName:(NSString *)albumName {
    // 1. 创建搜索集合
    PHFetchResult *result = [PHAssetCollection fetchAssetCollectionsWithType:PHAssetCollectionTypeAlbum subtype:PHAssetCollectionSubtypeAlbumRegular options:nil];
    
    // 2. 遍历搜索集合并取出对应的相册，返回当前的相册changeRequest
    for (PHAssetCollection *assetCollection in result) {
        if ([assetCollection.localizedTitle containsString:albumName]) {
            PHAssetCollectionChangeRequest *collectionRuquest = [PHAssetCollectionChangeRequest changeRequestForAssetCollection:assetCollection];
            return collectionRuquest;
        }
    }
    
    // 3. 如果不存在，创建一个名字为albumName的相册changeRequest
    PHAssetCollectionChangeRequest *collectionRequest = [PHAssetCollectionChangeRequest creationRequestForAssetCollectionWithTitle:albumName];
    return collectionRequest;
}

- (void)saveVideo:(NSString *)path {
    [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
        PHAssetCollectionChangeRequest * collectionChangeRequest = [self getCurrentPhotoCollectionWithAlbumName:@"test"];

        PHAssetChangeRequest * assetRequest = [PHAssetChangeRequest creationRequestForAssetFromVideoAtFileURL:[NSURL fileURLWithPath:path]];

        PHObjectPlaceholder * placeholder = [assetRequest placeholderForCreatedAsset];
        [collectionChangeRequest addAssets:@[placeholder]];
    } completionHandler:^(BOOL success, NSError * _Nullable error) {
        NSLog(@"finished adding %@ error:%@", @(success), error);
    }];
    


}


@end





@interface FFmpegVideoRecorder ()


@end
@implementation FFmpegVideoRecorder





@end


//
//
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <math.h>
//
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif
//
//
//
//#undef exit
//
///* 5 seconds stream duration */
//#define STREAM_DURATION   5.0
//#define STREAM_FRAME_RATE 25 /* 25 images/s */
//#define STREAM_NB_FRAMES  ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
//#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */
//
//static int sws_flags = SWS_BICUBIC;
//
///**************************************************************/
///* audio output */
//
//float t, tincr, tincr2;
//int16_t *samples;
//uint8_t *audio_outbuf;
//int audio_outbuf_size;
//int audio_input_frame_size;
//
///*
// * add an audio output stream
// */
//static AVStream *add_audio_stream(AVFormatContext *oc, enum AVCodecID codec_id)
//{
//    AVStream *st;
//    AVCodecParameters * params;
//
////    AVCodec
//
//    AVCodec *codec = avcodec_find_encoder(codec_id);
//    st = avformat_new_stream(oc, codec);
//
//    if (!st) {
//        fprintf(stderr, "Could not alloc stream\n");
//        exit(1);
//    }
//
//    params = st->codecpar;
//    params->codec_id = codec_id;
//    params->codec_type = AVMEDIA_TYPE_AUDIO;
//
//    /* put sample parameters */
//    params->bit_rate = 64000;
//    params->sample_rate = 44100;
//    params->channels = 2;
//    return st;
//}
//
//static void open_audio(AVFormatContext *oc, AVStream *st)
//{
//    AVCodecParameters * params;
//    AVCodec *codec;
//
//    params = st->codecpar;
//
//    /* find the audio encoder */
//    codec = avcodec_find_encoder(params->codec_id);
//    if (!codec) {
//        fprintf(stderr, "codec not found\n");
//        exit(1);
//    }
//
//
//    AVCodecContext * ctx = avcodec_alloc_context3(codec);
//    int result = avcodec_parameters_to_context(ctx, params);
//    if (result < 0) {
//        fprintf(stderr, "avcodec_parameters_to_context error\n");
//        exit(1);
//    }
//
//    /* open it */
//    if (avcodec_open2(ctx, codec, NULL) < 0) {
//        fprintf(stderr, "could not open codec\n");
//        exit(1);
//    }
//
//    /* init signal generator */
//    t = 0;
//    tincr = 2 * M_PI * 110.0 / params->sample_rate;
//    /* increment frequency by 110 Hz per second */
//    tincr2 = 2 * M_PI * 110.0 / params->sample_rate / params->sample_rate;
//
//    audio_outbuf_size = 10000;
//    audio_outbuf = av_malloc(audio_outbuf_size);
//
//    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
//       support to compute the input frame size in samples */
//    if (params->frame_size <= 1) {
//        audio_input_frame_size = audio_outbuf_size / params->channels;
//        switch(st->codecpar->codec_id) {
//        case AV_CODEC_ID_PCM_S16LE:
//        case AV_CODEC_ID_PCM_S16BE:
//        case AV_CODEC_ID_PCM_U16LE:
//        case AV_CODEC_ID_PCM_U16BE:
//            audio_input_frame_size >>= 1;
//            break;
//        default:
//            break;
//        }
//    } else {
//        audio_input_frame_size = params->frame_size;
//    }
//    samples = av_malloc(audio_input_frame_size * 2 * params->channels);
//}
//
///* prepare a 16 bit dummy audio frame of 'frame_size' samples and
//   'nb_channels' channels */
//static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
//{
//    int j, i, v;
//    int16_t *q;
//
//    q = samples;
//    for(j=0;j<frame_size;j++) {
//        v = (int)(sin(t) * 10000);
//        for(i = 0; i < nb_channels; i++)
//            *q++ = v;
//        t += tincr;
//        tincr += tincr2;
//    }
//}
//
//static void write_audio_frame(AVFormatContext *oc, AVStream *st)
//{
//    AVCodecContext *c;
//    AVPacket pkt;
//    av_init_packet(&pkt);
//
//    c = st->codec;
//
//    get_audio_frame(samples, audio_input_frame_size, c->channels);
//
//
//    avcodec_send_frame(<#AVCodecContext *avctx#>, <#const AVFrame *frame#>)
//
//    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);
//
//    if (c->coded_frame->pts != AV_NOPTS_VALUE)
//        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
//    pkt.flags |= PKT_FLAG_KEY;
//    pkt.stream_index= st->index;
//    pkt.data= audio_outbuf;
//
//    /* write the compressed frame in the media file */
//    if (av_interleaved_write_frame(oc, &pkt) != 0) {
//        fprintf(stderr, "Error while writing audio frame\n");
//        exit(1);
//    }
//}
//
//static void close_audio(AVFormatContext *oc, AVStream *st)
//{
//    avcodec_close(st->codec);
//
//    av_free(samples);
//    av_free(audio_outbuf);
//}
//
///**************************************************************/
///* video output */
//
//AVFrame *picture, *tmp_picture;
//uint8_t *video_outbuf;
//int frame_count, video_outbuf_size;
//
///* add a video output stream */
//static AVStream *add_video_stream(AVFormatContext *oc, int codec_id)
//{
//    AVCodecContext *c;
//    AVStream *st;
//
//    st = av_new_stream(oc, 0);
//    if (!st) {
//        fprintf(stderr, "Could not alloc stream\n");
//        exit(1);
//    }
//
//    c = st->codec;
//    c->codec_id = codec_id;
//    c->codec_type = CODEC_TYPE_VIDEO;
//
//    /* put sample parameters */
//    c->bit_rate = 400000;
//    /* resolution must be a multiple of two */
//    c->width = 352;
//    c->height = 288;
//    /* time base: this is the fundamental unit of time (in seconds) in terms
//       of which frame timestamps are represented. for fixed-fps content,
//       timebase should be 1/framerate and timestamp increments should be
//       identically 1. */
//    c->time_base.den = STREAM_FRAME_RATE;
//    c->time_base.num = 1;
//    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
//    c->pix_fmt = STREAM_PIX_FMT;
//    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
//        /* just for testing, we also add B frames */
//        c->max_b_frames = 2;
//    }
//    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
//        /* Needed to avoid using macroblocks in which some coeffs overflow.
//           This does not happen with normal video, it just happens here as
//           the motion of the chroma plane does not match the luma plane. */
//        c->mb_decision=2;
//    }
//    // some formats want stream headers to be separate
//    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
//        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
//
//    return st;
//}
//
//static AVFrame *alloc_picture(int pix_fmt, int width, int height)
//{
//    AVFrame *picture;
//    uint8_t *picture_buf;
//    int size;
//
//    picture = avcodec_alloc_frame();
//    if (!picture)
//        return NULL;
//    size = avpicture_get_size(pix_fmt, width, height);
//    picture_buf = av_malloc(size);
//    if (!picture_buf) {
//        av_free(picture);
//        return NULL;
//    }
//    avpicture_fill((AVPicture *)picture, picture_buf,
//                   pix_fmt, width, height);
//    return picture;
//}
//
//static void open_video(AVFormatContext *oc, AVStream *st)
//{
//    AVCodec *codec;
//    AVCodecContext *c;
//
//    c = st->codec;
//
//    /* find the video encoder */
//    codec = avcodec_find_encoder(c->codec_id);
//    if (!codec) {
//        fprintf(stderr, "codec not found\n");
//        exit(1);
//    }
//
//    /* open the codec */
//    if (avcodec_open(c, codec) < 0) {
//        fprintf(stderr, "could not open codec\n");
//        exit(1);
//    }
//
//    video_outbuf = NULL;
//    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
//        /* allocate output buffer */
//        /* XXX: API change will be done */
//        /* buffers passed into lav* can be allocated any way you prefer,
//           as long as they're aligned enough for the architecture, and
//           they're freed appropriately (such as using av_free for buffers
//           allocated with av_malloc) */
//        video_outbuf_size = 200000;
//        video_outbuf = av_malloc(video_outbuf_size);
//    }

//    /* allocate the encoded raw picture */
//    picture = alloc_picture(c->pix_fmt, c->width, c->height);
//    if (!picture) {
//        fprintf(stderr, "Could not allocate picture\n");
//        exit(1);
//    }
//
//    /* if the output format is not YUV420P, then a temporary YUV420P
//       picture is needed too. It is then converted to the required
//       output format */
//    tmp_picture = NULL;
//    if (c->pix_fmt != PIX_FMT_YUV420P) {
//        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
//        if (!tmp_picture) {
//            fprintf(stderr, "Could not allocate temporary picture\n");
//            exit(1);
//        }
//    }
//}
//
///* prepare a dummy image */
//static void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
//{
//    int x, y, i;
//
//    i = frame_index;
//
//    /* Y */
//    for(y=0;y<height;y++) {
//        for(x=0;x<width;x++) {
//            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
//        }
//    }
//
//    /* Cb and Cr */
//    for(y=0;y<height/2;y++) {
//        for(x=0;x<width/2;x++) {
//            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
//            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
//        }
//    }
//}
//
//static void write_video_frame(AVFormatContext *oc, AVStream *st)
//{
//    int out_size, ret;
//    AVCodecContext *c;
//    static struct SwsContext *img_convert_ctx;
//
//    c = st->codec;
//
//    if (frame_count >= STREAM_NB_FRAMES) {
//        /* no more frame to compress. The codec has a latency of a few
//           frames if using B frames, so we get the last frames by
//           passing the same picture again */
//    } else {
//        if (c->pix_fmt != PIX_FMT_YUV420P) {
//            /* as we only generate a YUV420P picture, we must convert it
//               to the codec pixel format if needed */
//            if (img_convert_ctx == NULL) {
//                img_convert_ctx = sws_getContext(c->width, c->height,
//                                                 PIX_FMT_YUV420P,
//                                                 c->width, c->height,
//                                                 c->pix_fmt,
//                                                 sws_flags, NULL, NULL, NULL);
//                if (img_convert_ctx == NULL) {
//                    fprintf(stderr, "Cannot initialize the conversion context\n");
//                    exit(1);
//                }
//            }
//            fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
//            sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
//                      0, c->height, picture->data, picture->linesize);
//        } else {
//            fill_yuv_image(picture, frame_count, c->width, c->height);
//        }
//    }
//
//
//    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
//        /* raw video case. The API will change slightly in the near
//           futur for that */
//        AVPacket pkt;
//        av_init_packet(&pkt);
//
//        pkt.flags |= PKT_FLAG_KEY;
//        pkt.stream_index= st->index;
//        pkt.data= (uint8_t *)picture;
//        pkt.size= sizeof(AVPicture);
//
//        ret = av_interleaved_write_frame(oc, &pkt);
//    } else {
//        /* encode the image */
//        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
//        /* if zero size, it means the image was buffered */
//        if (out_size > 0) {
//            AVPacket pkt;
//            av_init_packet(&pkt);
//
//            if (c->coded_frame->pts != AV_NOPTS_VALUE)
//                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
//            if(c->coded_frame->key_frame)
//                pkt.flags |= PKT_FLAG_KEY;
//            pkt.stream_index= st->index;
//            pkt.data= video_outbuf;
//            pkt.size= out_size;
//
//            /* write the compressed frame in the media file */
//            ret = av_interleaved_write_frame(oc, &pkt);
//        } else {
//            ret = 0;
//        }
//    }
//    if (ret != 0) {
//        fprintf(stderr, "Error while writing video frame\n");
//        exit(1);
//    }
//    frame_count++;
//}
//
//static void close_video(AVFormatContext *oc, AVStream *st)
//{
//    avcodec_close(st->codec);
//    av_free(picture->data[0]);
//    av_free(picture);
//    if (tmp_picture) {
//        av_free(tmp_picture->data[0]);
//        av_free(tmp_picture);
//    }
//    av_free(video_outbuf);
//}
//
///**************************************************************/
///* media file output */
//
//int main(int argc, char **argv)
//{
//    const char *filename;
//    AVOutputFormat *fmt;
//    AVFormatContext *oc;
//    AVStream *audio_st, *video_st;
//    double audio_pts, video_pts;
//    int i;
//
//    /* initialize libavcodec, and register all codecs and formats */
//    av_register_all();
//
//    if (argc != 2) {
//        printf("usage: %s output_file\n"
//               "API example program to output a media file with libavformat.\n"
//               "The output format is automatically guessed according to the file extension.\n"
//               "Raw images can also be output by using '%%d' in the filename\n"
//               "\n", argv[0]);
//        exit(1);
//    }
//
//    filename = argv[1];
//
//    /* auto detect the output format from the name. default is
//       mpeg. */
//    fmt = guess_format(NULL, filename, NULL);
//    if (!fmt) {
//        printf("Could not deduce output format from file extension: using MPEG.\n");
//        fmt = guess_format("mpeg", NULL, NULL);
//    }
//    if (!fmt) {
//        fprintf(stderr, "Could not find suitable output format\n");
//        exit(1);
//    }
//
//    /* allocate the output media context */
//    oc = avformat_alloc_context();
//    if (!oc) {
//        fprintf(stderr, "Memory error\n");
//        exit(1);
//    }
//    oc->oformat = fmt;
//    snprintf(oc->filename, sizeof(oc->filename), "%s", filename);
//
//    /* add the audio and video streams using the default format codecs
//       and initialize the codecs */
//    video_st = NULL;
//    audio_st = NULL;
//    if (fmt->video_codec != CODEC_ID_NONE) {
//        video_st = add_video_stream(oc, fmt->video_codec);
//    }
//    if (fmt->audio_codec != CODEC_ID_NONE) {
//        audio_st = add_audio_stream(oc, fmt->audio_codec);
//    }
//
//    /* set the output parameters (must be done even if no
//       parameters). */
//    if (av_set_parameters(oc, NULL) < 0) {
//        fprintf(stderr, "Invalid output format parameters\n");
//        exit(1);
//    }
//
//    dump_format(oc, 0, filename, 1);
//
//    /* now that all the parameters are set, we can open the audio and
//       video codecs and allocate the necessary encode buffers */
//    if (video_st)
//        open_video(oc, video_st);
//    if (audio_st)
//        open_audio(oc, audio_st);
//
//    /* open the output file, if needed */
//    if (!(fmt->flags & AVFMT_NOFILE)) {
//        if (url_fopen(&oc->pb, filename, URL_WRONLY) < 0) {
//            fprintf(stderr, "Could not open '%s'\n", filename);
//            exit(1);
//        }
//    }
//
//
//    /* write the stream header, if any */
//    av_write_header(oc);
//
//    for(;;) {
//        /* compute current audio and video time */
//        if (audio_st)
//            audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
//        else
//            audio_pts = 0.0;
//
//        if (video_st)
//            video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
//        else
//            video_pts = 0.0;
//
//        if ((!audio_st || audio_pts >= STREAM_DURATION) &&
//            (!video_st || video_pts >= STREAM_DURATION))
//            break;
//
//        /* write interleaved audio and video frames */
//        if (!video_st || (video_st && audio_st && audio_pts < video_pts)) {
//            write_audio_frame(oc, audio_st);
//        } else {
//            write_video_frame(oc, video_st);
//        }
//    }
//
//    /* write the trailer, if any.  the trailer must be written
//     * before you close the CodecContexts open when you wrote the
//     * header; otherwise write_trailer may try to use memory that
//     * was freed on av_codec_close() */
//    av_write_trailer(oc);
//
//    /* close each codec */
//    if (video_st)
//        close_video(oc, video_st);
//    if (audio_st)
//        close_audio(oc, audio_st);
//
//    /* free the streams */
//    for(i = 0; i < oc->nb_streams; i++) {
//        av_freep(&oc->streams[i]->codec);
//        av_freep(&oc->streams[i]);
//    }
//
//    if (!(fmt->flags & AVFMT_NOFILE)) {
//        /* close the output file */
//        url_fclose(oc->pb);
//    }
//
//    /* free the stream */
//    av_free(oc);
//
//    return 0;
//}
