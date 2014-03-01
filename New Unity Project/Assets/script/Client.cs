#define _DEBUG

using UnityEngine;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using System;
using System.Threading;
using System.Collections.Generic;
using TcpCSFramework;
using System.Text;

public class Client : MonoBehaviour {
    public static UInt32 Sys_GetTime()
	{
	    return (UInt32)System.Environment.TickCount;
	}
	public static void Sys_Log(object obj,params object[] para)
	{
	    Debug.Log(string.Format(obj.ToString(),para));
	}
    void ClientConn(object sender, NetEventArgs e)
    {
        m_lstReadTime = Sys_GetTime();
        m_lstSendTime = Sys_GetTime();
        m_state = State.ST_Connected;
        Sys_Log("Connected:"+e.Client.ClientSocket.RemoteEndPoint);
    }
    void ClientClose(object sender, NetEventArgs e)
    {
        m_state = State.ST_Disconnected;
        Sys_Log("Disconnectd:" + e.Client.ClientSocket.RemoteEndPoint
            + "\n" + e.Client.TypeOfExit);
    }
    static string getString(byte[] buff)
    {
        string ret = "";
        foreach (var item in buff)
        {
            ret += string.Format("{0:X} ", item);
        }
        return string.Format("<== 0x{0}", ret);
    }
    public Game m_game;
    public Other m_other;
    void RecvData(object sender, NetEventArgs e)
    {
        Sys_Log(getString(e.Client.RecvPacket.GetReadData()));
        m_lstReadTime = Sys_GetTime();
        UInt16 cmd=0;
        CmdPacket packet = e.Client.RecvPacket;
        packet.ReadUShort(out cmd);
        switch (cmd)
        {
            case Proto.Keep_Alive:
                {
                    UInt32 time=0;
                    if (packet.ReadUInt(out time))
                    {
                        CmdPacket tpacket = new CmdPacket();
                        tpacket.WriteUShort(Proto.Keep_Alive_Ack);
                        tpacket.WriteUInt(time);
                        send(tpacket);
                    }
                    break;
                }
            case Proto.Keep_Alive_Ack:
                {
                    UInt32 time = 0;
                    if (packet.ReadUInt(out time))
                        m_rtt = Sys_GetTime() - time;
                    Sys_Log("herea:"+(int)m_rtt);
                    break;
                }
            case Proto.Synch_Pos:
                {
                    //同步位置和速度
                    if (!m_game.m_bSer)
                    {
                        UInt64 time;
                        packet.ReadUInt64(out time);
                        float x, z, vx, vz;
                        packet.ReadFloat(out x);
                        packet.ReadFloat(out z);
                        packet.ReadFloat(out vx);
                        packet.ReadFloat(out vz);
                        m_other.change(time, x, z, vx, vz);
                    }
                    break;
                }
            case Proto.S_GameShot:
                {
                    //同步操作
                    float x = 0, y = 0;
                    packet.ReadFloat(out x);
                    packet.ReadFloat(out y);
                    if (m_game.m_bSer)
                        m_other.shot(x, y);
                    break;
                }
            case Proto.S_GameInit:
                {
                    m_state = State.ST_GameInit;
                    break;
                }
            case Proto.S_GameStart:
                {
                    m_state = State.ST_GameRunning;
                    char bSer;
                    if (packet.ReadByte(out bSer))
                    {
                       // m_game.m_bSer = bSer != 0;
                    }
                    char bottom='\0';
                    if(packet.ReadByte(out bottom))
                        m_game.m_bottom=(bottom!=0);
                    m_game.start();
                    break;
                }
        }
    }
    public TcpCli m_client = new TcpCli();
    void Awake()
    {
        m_game.m_bSer = true;
        m_client.ReceivedDatagram += new NetEvent(RecvData);
        m_client.DisConnectedServer += new NetEvent(ClientClose);
        m_client.ConnectedServer += new NetEvent(ClientConn);
    }
    public enum State { ST_None, ST_Connected, ST_Disconnected, ST_WaitingToMatch, ST_GameInit, ST_WaitingToStart, ST_GameRunning };
    public State m_state = State.ST_Disconnected;
    void OnGUI()
    {
        GUILayout.Label("Ping:" + m_rtt);
        GUILayout.Label(m_state.ToString());
        if (m_state == State.ST_Disconnected)
        {
            if (GUILayout.Button("Connect"))
            {
                //Security.PrefetchSocketPolicy("127.0.0.1", 5001);
                m_client.Connect("127.0.0.1", 5001);
            }
        }
        else
        {
            if (GUILayout.Button("Disconnect"))
            {
                m_client.Close();
                m_state = State.ST_Disconnected;
            }
            switch (m_state)
            {
                case State.ST_Connected:
                    if (GUILayout.Button("Automatch"))
                    {
                        //send automatch
                        CmdPacket packet = new CmdPacket();
                        packet.WriteUShort(Proto.C_AutoMatch);
                        send(packet);
                        m_state = State.ST_WaitingToMatch;
                    }
                    break;
                case State.ST_WaitingToMatch:
                    if (GUILayout.Button("Cancel Match"))
                    {
                        CmdPacket packet = new CmdPacket();
                        packet.WriteUShort(Proto.C_UAutoMatch);
                        send(packet);
                        m_state = State.ST_Connected;
                    }
                    break;
                case State.ST_GameInit:
                    {
                        if (GUILayout.Button("Ready"))
                        {
                            CmdPacket packet = new CmdPacket();
                            packet.WriteUShort(Proto.C_GameReady);
                            send(packet);
                            m_state = State.ST_WaitingToStart;
                        }
                        break;
                    }
                case State.ST_WaitingToStart:
                case State.ST_GameRunning:
                    break;
            }
        }
    }
    void FixedUpdate()
    {
        if (m_state != State.ST_Disconnected)
        {
            if (Sys_GetTime() - m_lstSendTime > 10 * 1000)
            {
                CmdPacket packet = new CmdPacket();
                packet.WriteUShort(Proto.Keep_Alive);
                packet.WriteUInt(Sys_GetTime());
                send(packet);
            }
            if (Sys_GetTime() - m_lstReadTime > 30 * 1000)
            {
                //dead 
            }
        }
    }
    public void sendInput(float x, float y)
    {
        CmdPacket packet = new CmdPacket();
        packet.WriteUShort(Proto.C_GameShot);
        packet.WriteFloat(x);
        packet.WriteFloat(y);
        send(packet);
    }
    private UInt32 m_lstSendTime;
    private UInt32 m_lstReadTime;
    public UInt32 m_rtt;
    public void send(CmdPacket packet)
    {
        m_client.Send(packet);
        m_lstSendTime = Sys_GetTime();
    }
    const int NET_KEEP_ALIVE_INTERVAL = 10 * 1000;
    const int NET_CONNECT_TIMEOUT = 30 * 1000;
#if _DEBUG
	const int NET_TIMEOUT			= 900*1000;
#else
	const int NET_TIMEOUT			= 90*1000;
#endif
}
