using UnityEngine;
using System.Collections;
using TcpCSFramework;

public class InputMgr : MonoBehaviour {
    public Transform selfGun;
    public Transform ball;
    public float rate = 10;
    public float cameraMoveSpeed = 10;
    public bool m_enabled = false;
    public Client m_client;
    public Game m_game;
	// Update is called once per frame
	void Update () {
        Vector3 vec = ball.rigidbody.velocity;
        if (vec.x > 50) vec.x = 50;
        if (vec.x < -50) vec.x = -50;
        if (vec.z > 50) vec.z = 50;
        if (vec.z < -50) vec.z = -50;
        ball.rigidbody.velocity = vec;
        Vector3 cameraVec = Camera.main.transform.position;
        if (cameraVec.x < -3) cameraVec.x = -3;
        if (cameraVec.x > 3) cameraVec.x = 3;
        Camera.main.transform.position = cameraVec;
        if (!m_enabled)
            return;
        if (m_game.m_bottom&&ball.position.z <= 0||!m_game.m_bottom&&ball.position.z>=0)
        {
            if (Input.GetMouseButtonUp(0))
            {
                Vector3 direction = ball.position - selfGun.position;
                direction.y = 0;
                Debug.Log("self:" + direction);
                ball.rigidbody.AddForce(direction * rate, ForceMode.Impulse);
                m_client.sendInput(direction.x, direction.z);
            }
        }
        if (Input.GetKey(KeyCode.A))
            cameraVec.x += -1 * Time.deltaTime*cameraMoveSpeed;
        if (Input.GetKey(KeyCode.D)) 
            cameraVec.x += 1 * Time.deltaTime * cameraMoveSpeed;
	}
}