static GstPadProbeReturn osd_sink_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * probe_info, gpointer u_data)
{
    GstBuffer *buf = (GstBuffer *) probe_info->data;
    NvDsFrameMeta *frame_meta    = NULL;
    NvOSD_TextParams *text_params= NULL;
    NvOSD_RectParams *rect_params= NULL;
    NvDsObjectParams *obj_meta   = NULL;
    static GQuark _nvdsmeta_quark = 0;
 
 
    if (!_nvdsmeta_quark)
    {
        //g_quark_to_string
        _nvdsmeta_quark = g_quark_from_static_string (NVDS_META_STRING);
    }
 
    while ((gst_meta = gst_buffer_iterate_meta (buf, &state)))
    {
 
        if (!gst_meta_api_type_has_tag (gst_meta->info->api, _nvdsmeta_quark))
        {
            continue;
        }
 
        nvdsmeta = (NvDsMeta *) gst_meta;
        /* 只对解析类型的元感兴趣。 */
        if (nvdsmeta->meta_type != NVDS_META_FRAME_INFO)
        {
            continue;
        }
 
        frame_meta = (NvDsFrameMeta *) nvdsmeta->meta_data;
        if (frame_meta == NULL)
        {
            return GST_PAD_PROBE_OK;
        }
 
        frame_meta->num_strings = 0;
        num_rects = frame_meta->num_rects;
        for (rect_index = 0; rect_index < num_rects; rect_index++)
        {
            /* 现在，使用上述信息，构造一个用于显示在边界框顶端的字串，在这里构造。*/
            obj_meta = (NvDsObjectParams *) & frame_meta->obj_params[rect_index];
            rect_params = &(obj_meta->rect_params);
            if (rect_params)
            {
                rect_params->has_bg_color       = 1;
                rect_params->bg_color.red       = 0.5;
                rect_params->bg_color.green     = 0.0;
                rect_params->bg_color.blue      = 0.0;
                rect_params->bg_color.alpha     = 0.3;
 
                rect_params->border_color.red   = 1.0;
                rect_params->border_color.green = 1.0;
                rect_params->border_color.blue  = 0.0;
                rect_params->border_color.alpha = 1.0;
          }
 
 
          text_params = &(obj_meta->text_params);
          /* 丢弃管线的字串。测试表明一直为空。 */
          if (text_params->display_text)
          {
            g_free (text_params->display_text);
          }
 
          text_params->display_text = g_malloc0 (MAX_DISPLAY_LEN);
          g_snprintf (text_params->display_text, MAX_DISPLAY_LEN, "%02d-%s",
              rect_index, pgie_classes_str[obj_meta->class_id]);
          /* 设置字串显示效果 */
          text_params->x_offset = obj_meta->rect_params.left;
          text_params->y_offset = obj_meta->rect_params.top - 25;
 
          /* Font , font-color and font-size */
          text_params->font_params.font_name        = "Arial";
          text_params->font_params.font_size        = 12;
          text_params->font_params.font_color.red   = 1.0;
          text_params->font_params.font_color.green = 1.0;
          text_params->font_params.font_color.blue  = 1.0;
          text_params->font_params.font_color.alpha = 1.0;
 
          /* Text background color */
          text_params->set_bg_clr        = 1;
          text_params->text_bg_clr.red   = 0.0;
          text_params->text_bg_clr.green = 0.0;
          text_params->text_bg_clr.blue  = 0.5;
          text_params->text_bg_clr.alpha = 0.3;
 
          frame_meta->num_strings++;
      }
  }
    
    return GST_PAD_PROBE_OK;
}