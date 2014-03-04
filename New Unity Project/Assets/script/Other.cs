using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using System.Collections;

public class Other:MonoBehaviour
{
    public float rate = 10;
    public void shot(float vx,float vz)
    {
        m_shots.Enqueue(new Vector2(vx, vz));
    }
    public Game m_game;
    public Queue<Vector2> m_shots = new Queue<Vector2>();
    public void change(UInt64 time, float x, float z, float vx, float vz)
    {
        m_curTime = time;
        m_svrPos = new Vector2(x, z);
        m_svrVel = new Vector2(vx, vz);
    }
    public bool m_curChange = false;
    public UInt64 m_curTime=TimeMgr.getTimeStampMicro();
    public Vector2 m_svrPos;
    public Vector2 m_svrVel;
    public float m_rate = 0.01f;
    void FixedUpdate()
    {
        if (m_game.m_state == Game.State.ST_Running)
        {
            if (TimeMgr.getTimeStampMicro() > m_curTime)
            {
                if (!m_game.m_bSer)
                {
                    Vector2 cltPos = new Vector2(m_game.m_ball.position.x, m_game.m_ball.position.z);
                    Vector2 cltVel = new Vector2(m_game.m_ball.rigidbody.velocity.x, m_game.m_ball.rigidbody.velocity.z);
                    float deltaTime = (TimeMgr.getTimeStampMicro() - m_curTime) / 1000.0f;
                    Vector2 posB = m_svrPos + m_svrVel * deltaTime;
                    Vector2 pos = cltPos * (1 - m_rate) + posB * m_rate;
                    m_game.m_ball.position = new Vector3(pos.x, m_game.m_ball.position.y, pos.y);
                    Vector2 vel = cltVel * (1 - m_rate) + m_svrVel * m_rate;
                    Debug.Log(cltVel.y+ "," +cltVel.y*(1-m_rate));
                    m_game.m_ball.rigidbody.velocity = new Vector3(vel.x, m_game.m_ball.rigidbody.velocity.y, vel.y);
                    Debug.Log("CltPos:" + cltPos + ",SvrPos:" + m_svrPos + ",CltVel:" + cltVel
                        +",SvrVel:"+m_svrVel);
                }
            }
            if (m_shots.Count > 0)
            {
                Vector2 dir = m_shots.Dequeue();
                Vector3 vec = m_game.m_ball.rigidbody.velocity;
                if (m_game.m_bottom && m_game.m_ball.position.z >= 0 || !m_game.m_bottom && m_game.m_ball.position.z <= 0)
                {
                    Vector3 direction = new Vector3(dir.x, 0, dir.y);
                    m_game.m_ball.rigidbody.velocity = direction;
                }
            }
        }
    }
}
