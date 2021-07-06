#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("GStreamer start!\n");

	gst_init(&argc, &argv);
	//GstElement * pipeline0 = gst_parse_launch("filesrc location = H264_1080.mp4 ! qtdemux ! queue ! decodebin ! autovideosink", NULL);

	GstElement * pipeline0 = gst_parse_launch("imxcompositor_g2d name=comp sink_0::xpos=0 sink_0::ypos=0 sink_0::width=512 sink_0::height=300 sink_1::xpos=0 sink_1::ypos=300 sink_1::width=512 sink_1::height=300 sink_2::xpos=512 sink_2::ypos=0 sink_2::width=512 sink_2::height=300 sink_3::xpos=512 sink_3::ypos=300 sink_3::width=512 sink_3::height=300 ! video/x-raw,format=RGB16 ! waylandsink filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=512, height=300 ! comp.sink_0 filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=512, height=300 ! comp.sink_1 filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=512, height=300 ! comp.sink_2 filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=512, height=300 ! comp.sink_3", NULL);

	//GstElement * pipeline1 = gst_parse_launch("filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=320, height=240 ! waylandsink window-height=300 window-width=512", NULL);
	
	//GstElement * pipeline2 = gst_parse_launch("filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=320, height=240 ! waylandsink window-height=300 window-width=512", NULL);
	
	//GstElement * pipeline3 = gst_parse_launch("filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=320, height=240 ! waylandsink window-height=300 window-width=512", NULL);
	
	//GstElement * pipeline4 = gst_parse_launch("filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=320, height=240 ! waylandsink window-height=300 window-width=512", NULL);
	
	//GstElement * pipeline5 = gst_parse_launch("filesrc location=H265_1080.mp4 typefind=true ! video/quicktime ! aiurdemux ! queue max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=320, height=240 ! waylandsink window-height=300 window-width=512", NULL);
	
	gst_element_set_state(pipeline0, GST_STATE_PLAYING);
	
	//gst_element_set_state(pipeline1, GST_STATE_PLAYING);
	
	//gst_element_set_state(pipeline2, GST_STATE_PLAYING);
	
	//gst_element_set_state(pipeline3, GST_STATE_PLAYING);
	
	//gst_element_set_state(pipeline4, GST_STATE_PLAYING);
	
	//gst_element_set_state(pipeline5, GST_STATE_PLAYING);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}




