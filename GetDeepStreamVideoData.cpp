
static GstPadProbeReturn osd_sink_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * probe_info, gpointer u_data)
 
{
   GstBuffer *gstbuf = (GstBuffer *) probe_info->data;
   GstMapInfo map_info;
   NvBufSurface *nvsurface = NULL;
 
   if (gstbuf == NULL)
   {
      return NULL;
   }
 
   //memset(&map_info, 0, sizeof(map_info));
   if (!gst_buffer_map (gstbuf, &map_info, GST_MAP_READ))
   {
     g_print ("gst_buffer_map() error!");
     return NULL;
   }
 
   nvsurface =  *((NvBufSurface **) map_info.data);
   //nvsurface->width;
   //nvsurface->height;
 
   nPixcelByteCount = nvsurface->width*nvsurface->height*BYTES_ARGB;
   if (pFrameVideoDataBuffer == NULL)
   {
       pFrameVideoDataBuffer = malloc(nPixcelByteCount);
   }
 
   //NvBufMemType=2, 即：
   //MEM_CUDA_DEVICE, < Device memory allocated using cudaMalloc
   if (nvsurface->mem_type == MEM_CUDA_DEVICE)
   {
       //不能直接访问，必须复制出来。
       cudaMemcpy(pFrameVideoDataBuffer, nvsurface->buf_data[0], nPixcelByteCount, cudaMemcpyDeviceToHost);
   }
   else
   {
       memcpy(pFrameVideoDataBuffer, nvsurface->buf_data[0], nPixcelByteCount);
   }
 
   gst_buffer_unmap (gstbuf, &map_info);
 
   //进行一些处理。
   //free(pFrameVideoDataBuffer);
   return GST_PAD_PROBE_OK;
}
