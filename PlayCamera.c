#include <gst/gst.h>
#include <stdio.h>

/*
	#if(1)
	pipeline = gst_parse_launch("gst-launch-1.0 imxcompositor_g2d name=comp \
	sink_0::xpos=0 sink_0::ypos=0 sink_0::width=512 sink_0::height=300 \
	sink_1::xpos=0 sink_1::ypos=300 sink_1::width=512 sink_1::height=300 \
	sink_2::xpos=512 sink_2::ypos=0 sink_2::width=512 sink_2::height=300 \
	sink_3::xpos=512 sink_3::ypos=300 sink_3::width=512 sink_3::height=300 ! \
	video/x-raw,format=RGB16 ! waylandsink \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.67:554/cam/realmonitor?channel=1\&subtype=0\&unicast=true\&proto=Onvif latency=300 ! rtph265depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_0 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.67:554/cam/realmonitor?channel=1\&subtype=0\&unicast=true\&proto=Onvif latency=300 ! rtph265depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_1 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.67:554/cam/realmonitor?channel=1\&subtype=0\&unicast=true\&proto=Onvif latency=300 ! rtph265depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_2 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.67:554/cam/realmonitor?channel=1\&subtype=0\&unicast=true\&proto=Onvif latency=300 ! rtph265depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_3 ", NULL);
	printf("\n************DAHUA Camera is Playing************\n\n");
	#endif
*/
//gst-launch-1.0 rtspsrc location=rtsp://admin:cx123456@192.168.60.64 latency=300 ! rtph264depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=1024, height=600 ! waylandsink 

typedef struct _CustomData {
  GstElement *pipeline;
  GstElement *appSrc;
  GstElement *rtpDepay;
  GstElement *rtph265Parse;
  GstElement *vpuDecode;
  GstElement *imxShowG2d;
  GstElement *waySink;
  
  GMainLoop *loop;
  GstBus *bus;
} CustomData;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) 
{
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("Stream Ends\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);
        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);
        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
    }
    return TRUE;
}

/* Dynamically link */
static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
    GstPad *sinkpad;
    GstPadLinkReturn ret;
    GstElement *decoder = (GstElement *) data;
    /* We can now link this pad with the rtsp-decoder sink pad */
    g_print ("Dynamic pad created, linking source/demuxer\n");
    sinkpad = gst_element_get_static_pad (decoder, "sink");

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked (sinkpad)) {
        g_print("*** We are already linked ***\n");
        gst_object_unref (sinkpad);
        return;
    } else {
        g_print("proceeding to linking ...\n");
    }
    ret = gst_pad_link (pad, sinkpad);

    if (GST_PAD_LINK_FAILED (ret)) {
        //failed
        g_print("failed to link dynamically\n");
    } else {
        //pass
        g_print("dynamically link successful\n");
    }

    gst_object_unref (sinkpad);
}

int main(int argc, char *argv[])
{
	CustomData data;
	gchar *urlVedio1 = "rtsp://admin:cx123456@192.168.60.67:554/cam/realmonitor?channel=1\&subtype=0\&unicast=true\&proto=Onvif";
	
	/* Initialize GStreamer */
	gst_init (&argc, &argv);
	
	/* Create the empty pipeline */
	data.pipeline = gst_pipeline_new ("decode_pipe");
    if (!data.pipeline)
    {
        g_printerr ("pipeline created error.\n");
		return -1;
    }
	
	/* Create the elements */
	data.appSrc = gst_element_factory_make ("rtspsrc", "rtspsrc0");
	data.rtpDepay = gst_element_factory_make ("rtph265depay", "rtph265depay0");	/* H265 */
	data.rtph265Parse = gst_element_factory_make ("h265parse", "h265parse0");;
	data.vpuDecode = gst_element_factory_make ("vpudec", "vpudec0");
	data.imxShowG2d = gst_element_factory_make ("imxvideoconvert_g2d", "imxvideoconvert_g2d0");
	//Video Sink
	data.waySink = gst_element_factory_make ("waylandsink", "waylandsink0");
	
	/* Make sure: Every elements was created ok */
	if( !data.pipeline || !data.appSrc || !data.rtpDepay || !data.rtph265Parse || !data.vpuDecode || !data.imxShowG2d || !data.waySink )
	{
		g_printerr ("One of the elements wasn't created... Exiting\n");
        return -1;
	}
	
	/* element Ù–‘…Ë÷√ */
	//g_object_set (G_OBJECT (data.waySink), "sync", FALSE, NULL);
	g_object_set (data.appSrc, "location", urlVedio1,
			  "latency" , 300,  NULL);
			  
	/* Add Elements to the Bin */ 
	gst_bin_add_many (GST_BIN (data.pipeline), data.appSrc, data.rtpDepay, data.rtph265Parse, data.vpuDecode, data.imxShowG2d, data.waySink, NULL);
	
	/* Link confirmation */
	if (!gst_element_link_many (data.rtpDepay, data.rtph265Parse, data.vpuDecode, data.imxShowG2d, data.waySink, NULL))
	{
		g_warning ("Linking part (A)-1 Fail...");
		return -1;
    }
	if(! g_signal_connect (data.appSrc, "pad-added", G_CALLBACK (on_pad_added), data.rtpDepay))
    {
        g_warning ("Linking part (1) with part (A)-1 Fail...");
    }
	  
	data.loop = g_main_loop_new (NULL, FALSE);
	data.bus = gst_pipeline_get_bus (GST_PIPELINE (data.pipeline));
	gst_bus_add_watch (data.bus, bus_call, data.loop);
    gst_object_unref (data.bus);
	
	/* Run the pipeline */
    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    g_main_loop_run (data.loop);
	
	/* Free resources */
	g_main_loop_unref (data.loop);
	gst_object_unref (data.bus);
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (data.pipeline));
}

