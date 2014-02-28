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
    public static int Sys_GetTime()
	{
	    return System.Environment.TickCount;
	}
	public static void Sys_Log(object obj,params object[] para)
	{
	    Debug.Log(string.Format(obj.ToString(),para));
	}
    void ClientConn(object sender, NetEventArgs e)
    {
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
        UInt16 cmd=0;
        e.Client.RecvPacket.ReadUShort(ref cmd);
        switch (cmd)
        {
            case Proto.S_GameInit:
                {
                    m_state = State.ST_GameInit;
                    break;
                }
            case Proto.S_GameStart:
                {
                    char bottom='\0';
                    e.Client.RecvPacket.ReadByte(ref bottom);
                    m_game.start(bottom!=0);
                    m_state = State.ST_GameRunning;
                    break;
                }
            case Proto.S_GameShot:
                {
                    float x=0,y=0;
                    e.Client.RecvPacket.ReadFloat(ref x);
                    e.Client.RecvPacket.ReadFloat(ref y);
                    m_other.shot(x, y);
                    break;
                }
        }
    }
    public TcpCli m_client = new TcpCli();
    void Awake()
    {
        m_client.ReceivedDatagram += new NetEvent(RecvData);
        m_client.DisConnectedServer += new NetEvent(ClientClose);
        m_client.ConnectedServer += new NetEvent(ClientConn);
    }
    public enum State { ST_None, ST_Connected, ST_Disconnected, ST_WaitingToMatch, ST_GameInit, ST_WaitingToStart, ST_GameRunning };
    public State m_state = State.ST_Disconnected;
    void OnGUI()
    {
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
                        m_client.Send(packet);
                        m_state = State.ST_WaitingToMatch;
                    }
                    break;
                case State.ST_WaitingToMatch:
                    if (GUILayout.Button("Cancel Match"))
                    {
                        CmdPacket packet = new CmdPacket();
                        packet.WriteUShort(Proto.C_UAutoMatch);
                        m_client.Send(packet);
                        m_state = State.ST_Connected;
                    }
                    break;
                case State.ST_GameInit:
                    {
                        if (GUILayout.Button("Ready"))
                        {
                            CmdPacket packet = new CmdPacket();
                            packet.WriteUShort(Proto.C_GameReady);
                            m_client.Send(packet);
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
    public void sendInput(float x, float y)
    {
        CmdPacket packet = new CmdPacket();
        packet.WriteUShort(Proto.C_GameShot);
        packet.WriteFloat(x);
        packet.WriteFloat(y);
        m_client.Send(packet);
    }
    const int NET_KEEP_ALIVE_INTERVAL = 10 * 1000;
    const int NET_CONNECT_TIMEOUT = 30 * 1000;
#if _DEBUG
	const int NET_TIMEOUT			= 900*1000;
#else
	const int NET_TIMEOUT			= 90*1000;
#endif
}
