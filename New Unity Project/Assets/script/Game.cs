﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using TcpCSFramework;

public class Game: MonoBehaviour
{
    public Transform m_tranBottom;
    public Transform m_tranUp;
    public Transform m_tranBall;
    public Transform m_other;
    public Transform m_ball;
    public void start(bool bottom)
    {
        m_bottom = bottom;
        m_state = State.ST_Start;
    }
    private float m_time;
    public float m_gameTime = 30;
    public Client m_client;
    public InputMgr m_input;
    public enum State
    {
        ST_None, ST_Start, ST_Running, ST_End
    }
    public State m_state = State.ST_None;
    public bool m_bottom=false;
    void Update()
    {
        if (m_state == State.ST_Start)
        {
            m_state = State.ST_Running;
            m_time = 0;
            m_input.m_enabled = true;
            if (m_bottom)
            {
                m_other.position = m_tranUp.position;
                m_other.rotation = m_tranUp.rotation;
                Camera.main.transform.position = m_tranBottom.position;
                Camera.main.transform.rotation = m_tranBottom.rotation;
            }
            else
            {
                m_other.position = m_tranBottom.position;
                m_other.rotation = m_tranBottom.rotation;
                Camera.main.transform.position = m_tranUp.position;
                Camera.main.transform.rotation = m_tranUp.rotation;
            }
            m_ball.position = m_tranBall.position;
            m_ball.rotation = m_tranBall.rotation;
        }
        if (m_state ==State.ST_Running)
        {
            if (m_time > m_gameTime)
            {
                m_state=State.ST_End;
                CmdPacket packet = new CmdPacket();
                //packet.WriteUShort(Proto.C_AutoMatch);
                //m_client.m_client.Send(packet);
            }
            m_time += Time.deltaTime;
            //同步位置和速度

        }
    }
}
