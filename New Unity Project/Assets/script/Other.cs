using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using System.Collections;

public class Other:MonoBehaviour
{
    public Transform m_ball;
    public float rate = 10;
    public void shot(float x, float y)
    {
        m_shots.Enqueue(new Vector2(x, y));
    }
    public Game m_game;
    public Queue<Vector2> m_shots = new Queue<Vector2>();
    public void change(UInt64 time, float x, float z, float vx, float vz)
    {
        m_curChange = true;
        m_curTime = time;
        m_curPos = new Vector3(x, 0, z);
        m_curVel = new Vector3(vx, 0, vz);
    }
    public bool m_curChange = false;
    public UInt64 m_curTime;
    public Vector3 m_curPos;
    public Vector3 m_curVel;
    void FixedUpdate()
    {
        if (m_curChange)
        {
            m_curChange = false;
            Debug.Log(TimeMgr.getTimeStamp() + "==" + m_curTime);
            if (TimeMgr.getTimeStamp() >= m_curTime)
            {
                float deltaTime=(TimeMgr.getTimeStamp()-m_curTime)/1000.0f;
                Vector3 targetB = m_curPos + m_curVel * deltaTime;
                //Vector3 vel = (targetB - m_ball.position).normalized * m_curVel.magnitude;
                if (deltaTime == 0)
                    deltaTime = 0.001f;
                Vector3 vel = (targetB - m_ball.position) / deltaTime;
                Debug.Log(vel);
                m_ball.rigidbody.velocity = vel;
            }
        }
        if (m_shots.Count > 0)
        {
            Vector2 dir = m_shots.Dequeue();
            Vector3 vec = m_ball.rigidbody.velocity;
            if (m_game.m_bottom && m_ball.position.z >= 0 || !m_game.m_bottom && m_ball.position.z <= 0)
            {
                Vector3 direction = new Vector3(dir.x, 0.21f, dir.y);
                m_ball.rigidbody.AddForce(direction * rate, ForceMode.Impulse);
            }
        }
    }
}
