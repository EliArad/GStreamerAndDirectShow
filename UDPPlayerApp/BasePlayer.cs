using DirectShowLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UDPPlayerApp
{
    public abstract class BasePlayer
    {

        public delegate void Callback(int code, object msg);

        protected IGraphBuilder m_graphBuilder;
        protected IMediaControl mediaControl = null;
        protected IBaseFilter m_UdpFilter = null;
        protected IBaseFilter m_ElecardAVCDecoderFilter = null;
        protected IBaseFilter m_ElecardPushMPEGDemuxFilter = null;
        protected IVideoWindow videoWindow = null;
        protected bool m_running;

        protected abstract void ConnectFilters();
        protected Form m_form;
        protected Panel m_panel;

        protected Callback pCallback;

        public BasePlayer(Form form , Panel panel, Callback p)
        {
            m_running = true;
            m_form = form;
            pCallback = p;
            m_panel = panel;
            m_graphBuilder = (IGraphBuilder)new FilterGraph();
            //Create the media control for controlling the graph
            mediaControl = (IMediaControl)m_graphBuilder;
        }

        protected void AddElecardAVCDecoder()
        {
            Type comtype;
            Guid guidFilter = new Guid("5C122C6D8FCC46F9AAB7DCFB0841E04D");
            comtype = Type.GetTypeFromCLSID(guidFilter);
            m_ElecardAVCDecoderFilter = (IBaseFilter)Activator.CreateInstance(comtype);

            int hr = m_graphBuilder.AddFilter(m_ElecardAVCDecoderFilter, "Elecard AVC Decoder Filter");

            DsError.ThrowExceptionForHR(hr);
        }

        protected void AddElecardMPEGPushDemux()
        {
            Type comtype;
            Guid guidFilter = new Guid("668EE184FD2D4C728E79439A35B438DE");
            comtype = Type.GetTypeFromCLSID(guidFilter);
            m_ElecardPushMPEGDemuxFilter = (IBaseFilter)Activator.CreateInstance(comtype);

            int hr = m_graphBuilder.AddFilter(m_ElecardPushMPEGDemuxFilter, "Elecard Push MPEG Demox Filter");

            DsError.ThrowExceptionForHR(hr);
        }

        public void Run()
        {
            mediaControl.Run();
        }
        public void Stop()
        {
            mediaControl.Stop();
        }
        protected int SetVideoWindow()
        {
            int hr;
            videoWindow = (IVideoWindow)m_graphBuilder;
            if (videoWindow == null)
                return 0;

            //Set the owener of the videoWindow to an IntPtr of some sort (the Handle of any control - could be a form / button etc.)
            hr = videoWindow.put_Owner(m_panel.Handle);
            DsError.ThrowExceptionForHR(hr);

            //Set the style of the video window
            hr = videoWindow.put_WindowStyle(WindowStyle.Child | WindowStyle.ClipChildren);
            DsError.ThrowExceptionForHR(hr);

            hr = videoWindow.put_MessageDrain(m_panel.Handle);


            // Position video window in client rect of main application window
            hr = videoWindow.SetWindowPosition(0, 0, m_panel.Width, m_panel.Height);
            DsError.ThrowExceptionForHR(hr);

            // Make the video window visible
            hr = videoWindow.put_Visible(OABool.True);
            DsError.ThrowExceptionForHR(hr);

            return hr;
        }

        protected void AddUDPFilter()
        {


            Type comtype;
            Guid guidFilter = new Guid("15CF9D07729A4CA49DAA95123FBA0EF2");
            comtype = Type.GetTypeFromCLSID(guidFilter);
            m_UdpFilter = (IBaseFilter)Activator.CreateInstance(comtype);

            int hr = m_graphBuilder.AddFilter(m_UdpFilter, "UDP Filter");

            DsError.ThrowExceptionForHR(hr);
        }
        public void Resize()
        {
            // Position video window in client rect of main application window
            int hr = videoWindow.SetWindowPosition(0, 0, m_panel.Width, m_panel.Height);
            DsError.ThrowExceptionForHR(hr);
        }
        public void Close()
        {
            try
            {
                mediaControl.Stop();

                if (m_ElecardAVCDecoderFilter != null)
                    Marshal.ReleaseComObject(m_ElecardAVCDecoderFilter);
                m_ElecardAVCDecoderFilter = null;

                if (m_UdpFilter != null)
                    Marshal.ReleaseComObject(m_UdpFilter);
                m_UdpFilter = null;


                if (m_ElecardPushMPEGDemuxFilter != null)
                    Marshal.ReleaseComObject(m_ElecardPushMPEGDemuxFilter);
                m_ElecardPushMPEGDemuxFilter = null;

                if (m_graphBuilder != null)
                    Marshal.ReleaseComObject(m_graphBuilder);
                m_graphBuilder = null;
            }
            catch (Exception err)
            {
                throw (new SystemException(err.Message));
            }
        }

    }
}
