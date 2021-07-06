#include <string.h>
#include <stdio.h>
#include <gst/gst.h>
  
#if(1)
typedef struct _CustomData {
  GstElement *pipeline;
  GstElement *video_sink;
  GMainLoop *loop;
  
  gboolean playing;  /* Playing or Paused */
  gdouble rate;      /* Current playback rate (can be negative) */
} CustomData;
  
/* Send seek event to change rate */
static void send_seek_event (CustomData *data) {
  gint64 position;
  GstFormat format = GST_FORMAT_TIME;
  GstEvent *seek_event;
  
  /* Obtain the current position, needed for the seek event */
  if (!gst_element_query_position (data->pipeline, format, &position)) {
	g_print("gst_element_query_position failed\n");
    g_printerr ("Unable to retrieve current position.\n");
    return;
  }
  
  /* Create the seek event */
  if (data->rate > 0) {
    seek_event = gst_event_new_seek (data->rate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
        GST_SEEK_TYPE_SET, position, GST_SEEK_TYPE_SET, -1);
  } else {
    seek_event = gst_event_new_seek (data->rate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
        GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, position);
  }
  
  if (data->video_sink == NULL) {
    /* If we have not done so, obtain the sink through which we will send the seek events */
    g_object_get (data->pipeline, "video-sink", &data->video_sink, NULL);
  }
  
  /* Send the event */
  gst_element_send_event (data->video_sink, seek_event);
  
  g_print ("Player Speed rate: %g\n", data->rate);
}
  
/* Process keyboard input */
static gboolean handle_keyboard (GIOChannel *source, GIOCondition cond, CustomData *data) {
  gchar *str = NULL;
  
  if (g_io_channel_read_line (source, &str, NULL, NULL, NULL) != G_IO_STATUS_NORMAL) {
    return TRUE;
  }
  
  switch (g_ascii_tolower (str[0])) {
  case 'p':
    data->playing = !data->playing;
    gst_element_set_state (data->pipeline, data->playing ? GST_STATE_PLAYING : GST_STATE_PAUSED);
    g_print ("Setting state to %s\n", data->playing ? "PLAYING" : "PAUSE");
    break;
  case 's':
	if(data->rate < 0.0)
	  data->rate *= -1;
    if (g_ascii_isupper (str[0])) {
      data->rate *= 2.0;
    } else {
      data->rate /= 2.0;
    }
    send_seek_event (data);
    break;
  case 'd':
    data->rate *= -1.0;
    send_seek_event (data);
    break;
  case 'n':
    if (data->video_sink == NULL) {
      /* If we have not done so, obtain the sink through which we will send the step events */
      g_object_get (data->pipeline, "video-sink", &data->video_sink, NULL);
    }
    
    gst_element_send_event (data->video_sink,
        gst_event_new_step (GST_FORMAT_BUFFERS, 1, data->rate, TRUE, FALSE));
    g_print ("Stepping one frame\n");
    break;
  case 'q':
    g_main_loop_quit (data->loop);
    break;
  default:
    break;
  }
  
  g_free (str);
  
  return TRUE;
}
  
int main(int argc, char *argv[]) {
  CustomData data;
  GstStateChangeReturn ret;
  GIOChannel *io_stdin;
  
  /* Initialize GStreamer */
  gst_init (&argc, &argv);
  
  /* Initialize our data structure */
  memset (&data, 0, sizeof (data));
  
  /* Print usage map */
  g_print (
    "USAGE: Choose one of the following options, then press enter:\n"
    " 'P' to toggle between PAUSE and PLAY\n"
    " 'S' to increase playback speed, 's' to decrease playback speed\n"
    " 'D' to toggle playback direction\n"
    " 'N' to move to next frame (in the current direction, better in PAUSE)\n"
    " 'Q' to quit\n");
  
  /* Build the pipeline */
  //data.pipeline = gst_parse_launch ("rtspsrc location=rtsp://192.168.60.109///home/data/RECORD/hd00/part00/ch3/1233_01_03_20201118_1933.MP4 ! \
	//				rtph264depay ! queue ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=1024, height=600 ! waylandsink", NULL);
  //data.pipeline = gst_parse_launch ("filesrc location=H264_1080.mp4  typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! \
	//				video/x-raw, format=RGB16, width=1024, height=600 ! waylandsink", NULL);
  //播放本地视频文件
  data.pipeline = gst_parse_launch ("playbin uri=file:////unit_tests/VPU/hantro/H264_1080.mp4", NULL);
					
					
  /* Add a keyboard watch so we get notified of keystrokes */
#ifdef _WIN32
  io_stdin = g_io_channel_win32_new_fd (fileno (stdin));
#else
  io_stdin = g_io_channel_unix_new (fileno (stdin));
#endif
  g_io_add_watch (io_stdin, G_IO_IN, (GIOFunc)handle_keyboard, &data);
  
  /* Start playing */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }
  data.playing = TRUE;
  data.rate = 1.0;
  
  /* Create a GLib Main Loop and set it to run */
  data.loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (data.loop);
  
  /* Free resources */
  g_main_loop_unref (data.loop);
  g_io_channel_unref (io_stdin);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  if (data.video_sink != NULL)
    gst_object_unref (data.video_sink);
  gst_object_unref (data.pipeline);
  return 0;
}

#endif


#if(0)

/* 视频快进接口 */
void play_fast_forward(GstElement *playbin_em)
{
    int64_t pos = 0;
    gboolean ret = gst_element_query_position(playbin_em,GST_FORMAT_TIME,&pos);
    if (0 == ret)
    {
        printf("gst_element_query_position fail\n");
        return;
    }
    printf("get pos=%lld\n",pos);
 
    gint64 duration = 0;
    ret = gst_element_query_duration(playbin_em,GST_FORMAT_TIME,&duration);
    if (0 == ret)
    {
        printf("gst_element_query_duration fail\n");
        return;
    }
    printf("get duration=%lld\n",duration);
 
    ret = gst_element_seek(playbin_em,4.0,GST_FORMAT_TIME,
                                    GST_SEEK_FLAG_FLUSH |GST_SEEK_FLAG_KEY_UNIT,
                                    GST_SEEK_TYPE_SET,
                                    pos,
                                    GST_SEEK_TYPE_SET,
                                    duration);
    if (0 == ret)
    {
        printf("play_fast_forward fail\n");
    }
    else
    {
        printf("play_fast_forward succ\n");
    }
}

/* 视频快退接口 */
void play_fast_reverse(GstElement *playbin_em)
{
    gint64 pos = 0;
    gboolean ret = gst_element_query_position(playbin_em,GST_FORMAT_TIME,&pos);
    if (0 == ret)
    {
        printf("gst_element_query_position fail\n");
        return;
    }
    printf("get pos=%lld\n",pos);
 
    ret = gst_element_seek(playbin_em,-4.0,GST_FORMAT_TIME,
                                    GST_SEEK_FLAG_FLUSH |GST_SEEK_FLAG_KEY_UNIT,
                                    GST_SEEK_TYPE_SET,
                                    0*GST_SECOND,
                                    GST_SEEK_TYPE_SET,
                                    pos);
    if (0 == ret)
    {
        printf("play_fast_reverse fail\n");
    }
    else
    {
        printf("play_fast_reverse succ\n");
    }
}

void play_normal_speed(GstElement *playbin_em)
{
    gint64 pos = 0;
    gboolean ret = gst_element_query_position(playbin_em,GST_FORMAT_TIME,&pos);
    if (0 == ret)
    {
        printf("gst_element_query_position fail\n");
        return;
    }
    printf("get pos=%lld\n",pos);
 
    gint64 duration = 0;
    ret = gst_element_query_duration(playbin_em,GST_FORMAT_TIME,&duration);
    if (0 == ret)
    {
        printf("gst_element_query_duration fail\n");
        return;
    }
    printf("get duration=%lld\n",duration);
 
    ret = gst_element_seek(playbin_em,1.0,GST_FORMAT_TIME,
                                    GST_SEEK_FLAG_FLUSH |GST_SEEK_FLAG_KEY_UNIT,
                                    GST_SEEK_TYPE_SET,
                                    pos,
                                    GST_SEEK_TYPE_SET,
                                    duration);
    if (0 == ret)
    {
        printf("play_normal_speed fail\n");
    }
    else
    {
        printf("play_normal_speed succ\n");
    }
}

int main(int argc, char *argv[]) {
  GstElement *pipeline;
  GMainLoop *loop;
  GstStateChangeReturn ret;
  
  /* Initialize GStreamer */
  gst_init (&argc, &argv);
 
  /* Build the pipeline */
  //data.pipeline = gst_parse_launch ("rtspsrc location=rtsp://192.168.60.109///home/data/RECORD/hd00/part00/ch3/1233_01_03_20201118_1933.MP4 ! \
	//				rtph264depay ! queue ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=1024, height=600 ! waylandsink", NULL);
  pipeline = gst_parse_launch ("filesrc location=H264_1080.mp4  typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! \
					video/x-raw, format=RGB16, width=1024, height=600 ! waylandsink", NULL);
					
  /* Start playing */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }
  
  	/* 设置快进播放 */
  //play_fast_forward(pipeline);
  /* 设置快退播放 */
  //play_fast_reverse(pipeline);
  /* 设置正常播放 */
  play_normal_speed(pipeline);
  
  /* Create a GLib Main Loop and set it to run */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);
  
  /* Free resources */
  g_main_loop_unref (loop);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  
  return 0;
}

#endif


