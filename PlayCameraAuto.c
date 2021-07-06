#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("GStreamer start!\n");
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	GMainLoop *loop;

	/* gst 初始化 */
	gst_init(&argc, &argv);
	//GstElement *
	
	//To test:去掉imxvideoconvert_g2d测试
	
	//海康威视摄像头
	#if(0)
	pipeline = gst_parse_launch("gst-launch-1.0 imxcompositor_g2d name=comp \
	sink_0::xpos=0 sink_0::ypos=0 sink_0::width=512 sink_0::height=300 \
	sink_1::xpos=0 sink_1::ypos=300 sink_1::width=512 sink_1::height=300 \
	sink_2::xpos=512 sink_2::ypos=0 sink_2::width=512 sink_2::height=300 \
	sink_3::xpos=512 sink_3::ypos=300 sink_3::width=512 sink_3::height=300 ! \
	video/x-raw,format=RGB16 ! waylandsink \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.64 latency=300 ! rtph264depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_0 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.64 latency=300 ! rtph264depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_1 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.64 latency=300 ! rtph264depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_2 \
	rtspsrc location=rtsp://admin:cx123456@192.168.60.64 latency=300 ! rtph264depay ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, width=512, height=300 ! comp.sink_3 ", NULL);
	printf("\n************HIKVISION Camera is Playing************\n\n");
	#endif
	
	//大华摄像头
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

	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the playing state.\n");
		gst_object_unref (pipeline);
		return -1;
	}
	
	/* Create a GLib Main Loop and set it to run */
	loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run(loop);
	
	/* Wait until error or EOS */
	bus = gst_element_get_bus (pipeline);
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* Free resources */
	g_main_loop_unref (loop);
	if (msg != NULL)
		gst_message_unref (msg);
	gst_object_unref (bus);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (pipeline);
	
	return 0;
}




