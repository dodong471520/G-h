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
    void FixedUpdate()
    {
        if (m_shots.Count > 0)
        {
            Vector2 dir = m_shots.Dequeue();
            Vector3 vec = m_ball.rigidbody.velocity;
            if (m_game.m_bottom && m_ball.position.z >= 0 || !m_game.m_bottom && m_ball.position.z <= 0)
            {
                Vector3 direction = new Vector3(dir.x, 0, dir.y);
                m_ball.rigidbody.AddForce(direction * rate, ForceMode.Impulse);
            }
        }
    }
}
