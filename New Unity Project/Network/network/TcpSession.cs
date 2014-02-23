#define _DEBUG

using System.Collections;
using System.Net;
using System.Net.Sockets;
using System;
using System.Threading;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Network
{
    public class TcpSession
    {
        [DllImport("kernel32")]
        static extern uint GetTickCount();
        enum TNetState
        {
            NET_STATE_CONNECTING,
            NET_STATE_WAITING_SEED,
            NET_STATE_CONNECTED,
            NET_STATE_DEAD,
            NET_STATE_ZERO,
        };
        TNetState m_state = TNetState.NET_STATE_ZERO;
        Socket m_clientSocket = null;
        uint m_connTime = 0;
        public bool Connect(string remote, int port)
        {
            try
            {
                m_clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                m_clientSocket.Connect(remote, port);
                if (m_clientSocket.Connected)
                {
                    m_clientSocket.Blocking = false;
                    m_clientSocket.ReceiveTimeout = 0;
                    m_clientSocket.SendTimeout = 0;
                    Sys_Log("OKKK");
                    m_state = TNetState.NET_STATE_CONNECTING;
                    m_connTime = Sys_GetTime();
                    m_lastSendTime = Sys_GetTime();
                    m_lastRecvTime = Sys_GetTime();

                    Thread newthread = new Thread(new ThreadStart(ListenThread));
                    newthread.Start();
                    return true;
                }
            }
            catch (System.Exception ex)
            {
                Sys_Log(ex.Message);
            }
            return false;
        }
        private static byte[] result = new byte[1024];
        uint m_lastSendTime;
        uint m_lastRecvTime;
        const int NET_KEEP_ALIVE_INTERVAL = 10 * 1000;
        const int NET_CONNECT_TIMEOUT = 30 * 1000;
#if _DEBUG
        const int NET_TIMEOUT = 900 * 1000;
#else
	const int NET_TIMEOUT			= 90*1000;
#endif
        void ListenThread()
        {
            while (true)
            {
                Process();
                if (m_state == TNetState.NET_STATE_DEAD)
                {
                    Sys_Log("Disconnected .......");
                    break;
                }
                else
                    System.Threading.Thread.Sleep(50);
            }
        }
        void Process()
        {
            DefProc();
            switch (m_state)
            {
                case TNetState.NET_STATE_CONNECTED:
                    while (GetPacket(m_recvCmdPkt))
                        OnReceive(m_recvCmdPkt);
                    break;

                case TNetState.NET_STATE_DEAD:
                    OnDisconnect();
                    Close();
                    break;

                case TNetState.NET_STATE_WAITING_SEED:
                    /* try get encrypt seed */
                    if (RecvPacket(m_recvNetPkt))
                    {
                        m_recvNetPkt.AttachCmdPacket(m_recvCmdPkt);
                        if (m_recvNetPkt.IsSysPacket())
                            HandleSysPacket(m_recvCmdPkt);
                    }
                    break;
                case TNetState.NET_STATE_CONNECTING:
                    if (HasExcept() ||
                        Sys_GetTime() - m_connTime > NET_CONNECT_TIMEOUT)
                    {
                        Sys_Log("connecting timeout");
                        Close();
                        OnConnect(false);
                        m_state = TNetState.NET_STATE_DEAD;
                        break;
                    }
                    if (CanWrite())
                    {
                        m_state = TNetState.NET_STATE_WAITING_SEED;
                        m_startTime = Sys_GetTime();
                        m_lastSendTime = m_startTime;
                        m_lastRecvTime = m_startTime;
                    }
                    break;
            }
        }
        bool GetPacket(CmdPacket packet)
        {
            while (RecvPacket(m_recvNetPkt))
            {
                /* attach a cmd packet for read */
                m_recvNetPkt.AttachCmdPacket(packet);
                if (m_recvNetPkt.IsSysPacket())
                {
                    /* if a system msg packet,handle it */
                    HandleSysPacket(packet);
                    continue;
                }
                else
                {
                    /* get a user data packet,return */
                    return true;
                }
            }
            return false;
        }

        int HandleSysPacket(CmdPacket packet)
        {
            short type = 0;
            uint param = 0;
            if (!packet.ReadShort(ref type)) return 0;
            switch (type)
            {
                case SYS_PACKET_KEEP_ALIVE:
                    if (!packet.ReadUInt(ref param)) return 0;
                    Sys_Log("recv keep alive msg");
                    m_sendCmdPkt.BeginWrite();
                    m_sendCmdPkt.WriteShort(SYS_PACKET_ALIVE_ACK);
                    m_sendCmdPkt.WriteInt((int)param);
                    SendPacket(m_sendCmdPkt, true, true);
                    break;
                case SYS_PACKET_ALIVE_ACK:
                    if (!packet.ReadUInt(ref param)) return 0;
                    Sys_Log("recv alive ack msg");
                    m_rtt = Sys_GetTime() - param;
                    break;
                case SYS_PACKET_SET_SEED:
                    if (!packet.ReadUInt(ref param)) return 0;
                    SetSeed(param);
                    m_state = TNetState.NET_STATE_CONNECTED;
                    OnConnect(true);
                    break;
                default:
                    return 0;
            }
            return type;
        }
        void SetSeed(uint seed)
        {
            Sys_Log("set seed = " + seed);
            if (seed == 0)
            {
                m_bEncrypt = false;
                return;
            }
            int i;
            for (i = 0; i < NetPacket.MAX_NET_SEED_SIZE; i++)
            {
                m_seed[i] = (byte)((seed % 255) + 1);
                seed /= 19;
            }
            m_seedLen = i;
            m_bEncrypt = true;
        }
        CmdPacket m_sendCmdPkt = new CmdPacket();
        CmdPacket m_recvCmdPkt = new CmdPacket();
        NetPacket m_sendNetPkt = new NetPacket();
        NetPacket m_recvNetPkt = new NetPacket();
        bool RecvPacket(NetPacket packet)
        {
            int ret;
            ret = packet.BeginRead(m_readBuff);
            if (ret > 0)
            {
                m_readBuff.RemoveRange(0, ret);
                if (packet.IsEncrypt())
                    packet.Decrypt(m_seed, m_seedLen);
                if (packet.IsCompress())
                    if (!packet.Uncompress()) return false;
                return true;
            }
            else if (ret == 0)
                return false;
            else
            {
                Sys_Log("error getcmd = -1");
                /* the packet flag error,not belong to this session */
                m_state = TNetState.NET_STATE_DEAD;
                return false;
            }
        }
        uint m_startTime = 0;
        bool CanRead()
        {
            List<Socket> exceptfds = new List<Socket>();
            exceptfds.Add(m_clientSocket);
            Socket.Select(exceptfds, null, null, 0);
            if (exceptfds.Contains(m_clientSocket))
                return true;
            return false;
        }
        bool CanWrite()
        {
            List<Socket> exceptfds = new List<Socket>();
            exceptfds.Add(m_clientSocket);
            Socket.Select(null, exceptfds, null, 0);
            if (exceptfds.Contains(m_clientSocket))
                return true;
            return false;
        }
        void OnConnect(bool ret)
        {
            Sys_Log("Connected");
        }
        void OnDisconnect()
        {
            Sys_Log("Disconnectd");
        }
        void OnReceive(CmdPacket packet)
        {
            short cmd = 0;
            packet.ReadShort(ref cmd);
            string ret = "";
            packet.ReadString(ref ret);
            Sys_Log(ret);
        }
        void Close()
        {
            Reset();
        }
        uint m_rtt = 0;
        void Reset()
        {
            m_rtt = 0;
            m_readBuff.Clear();
            m_writeBuf.Clear();
            m_clientSocket.Close();
            m_clientSocket = null;
            m_seedLen = 0;
            m_seq = 0;
            m_state = TNetState.NET_STATE_ZERO;
        }
        bool HasExcept()
        {
            List<Socket> exceptfds = new List<Socket>();
            exceptfds.Add(m_clientSocket);
            Socket.Select(null, null, exceptfds, 0);
            if (exceptfds.Contains(m_clientSocket))
                return true;
            return false;
        }
        void DefProc()
        {
            if (m_state != TNetState.NET_STATE_CONNECTED && m_state != TNetState.NET_STATE_WAITING_SEED) return;
            if (m_state == TNetState.NET_STATE_CONNECTED)
            {
                uint now = Sys_GetTime();
                if (now - m_lastSendTime > NET_KEEP_ALIVE_INTERVAL)
                {
                    KeepAlive(now);
                    m_lastSendTime = now;
                }
                if (now - m_lastRecvTime > NET_TIMEOUT)
                {
                    Sys_Log("tcp session dead reason = timeout");
                    m_state = TNetState.NET_STATE_DEAD;
                }
                FlushData();
            }
            ReadData();
        }
        List<byte> m_readBuff = new List<byte>();
        bool ReadData()
        {
            if (m_state == TNetState.NET_STATE_DEAD)
                return false;
            if (!CanRead())
                return false;
            byte[] bytes = new byte[NetPacket.MAX_BUFFER_SIZE];
            try
            {
                int ret = m_clientSocket.Receive(bytes);
                if (ret > 0)
                {
                    byte[] bytes2 = new byte[ret];
                    Array.Copy(bytes, bytes2, ret);
                    m_readBuff.AddRange(bytes2);
                    m_lastRecvTime = Sys_GetTime();
                    return true;
                }
                else if (ret == 0)
                {
                    return false;
                }
                else
                {
                    m_state = TNetState.NET_STATE_DEAD;
                }
            }
            catch (System.Exception ex)
            {
                m_state = TNetState.NET_STATE_DEAD;
                Sys_Log(ex.Message);
            }
            return false;
        }
        public static uint Sys_GetTime()
        {
            return GetTickCount();
        }
        public static void Sys_Log(System.Object str)
        {
            Console.WriteLine(str);
#if UnityEditor
            Debug.Log(str.ToString());
#endif
        }
        const byte PROTOCOL_FLAG = 5; // 0101
        const byte FLAG_ENCRYPT = 1;
        const byte FLAG_COMPRESS = 2;
        const byte FLAG_SYSTEM = 4;

        const short SYS_PACKET_SET_SEED = 1;
        const short SYS_PACKET_KEEP_ALIVE = 2;
        const short SYS_PACKET_ALIVE_ACK = 3;

        const ushort MAX_NET_PACKET_SEQ = 65000;
        const int NET_PACKET_HEAD_SIZE = 5;
        void KeepAlive(uint now)
        {
            m_sendCmdPkt.BeginWrite();
            m_sendCmdPkt.WriteShort(SYS_PACKET_KEEP_ALIVE);
            m_sendCmdPkt.WriteUInt(now);
            SendPacket(m_sendCmdPkt, true, true);
        }
        ushort m_seq = 0;
        bool m_bEncrypt = false;
        byte[] m_seed = new byte[16];
        int m_seedLen = 0;
        bool m_bCompress = true;
        List<byte> m_writeBuf = new List<byte>();
        public void SendPacket(CmdPacket packet)
        {
            SendPacket(packet, false, false);
        }
        bool SendPacket(CmdPacket packet, bool flush, bool sys)
        {
            lock (m_sendNetPkt)
            {
                if (m_state != TNetState.NET_STATE_CONNECTED)
                    return false;
                m_seq++;
                if (m_seq > MAX_NET_PACKET_SEQ) m_seq = 1;
                m_sendNetPkt.AddCmdPacket(packet);
                m_sendNetPkt.SetSeq(m_seq);
                if (sys)
                    m_sendNetPkt.SetSysPacket();
                else
                {
                    if (m_bEncrypt) m_sendNetPkt.Encrypt(m_seed, m_seedLen);
                    if (m_bCompress) m_sendNetPkt.Compress();
                }
                PushData(m_sendNetPkt.GetBuffer());
                if (flush) FlushData();
                return true;
            }
        }
        void PushData(List<byte> buff)
        {
            lock (m_writeBuf)
            {
                m_writeBuf.AddRange(buff);
            }
        }
        void FlushData()
        {
            lock (m_writeBuf)
            {
                if (m_state == TNetState.NET_STATE_DEAD)
                    return;
                if (m_writeBuf.Count == 0)
                    return;
                int ret = m_clientSocket.Send(m_writeBuf.ToArray());
                if (ret < 0)
                    m_state = TNetState.NET_STATE_DEAD;
                else if (ret == 0)
                {

                }
                else if (ret > 0)
                {
                    m_lastSendTime = Sys_GetTime();
                    m_writeBuf.RemoveRange(0, ret);
                }
            }
        }
    }
}
