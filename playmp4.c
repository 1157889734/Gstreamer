#include <gst/gst.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
  printf("GStreamer begin!\n");
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Build the pipeline */
  pipeline = gst_parse_launch("gst-launch-1.0 filesrc location=11.mp4 ! qtdemux ! queue ! h264parse ! mppvideodec ! waylandsink", 
				NULL);

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
 
  g_main_loop_run(g_main_loop_new(NULL, FALSE));

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Free resources */
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 0;
}



