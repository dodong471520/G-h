using UnityEngine;
using System.Collections;

public class InputMgr : MonoBehaviour {
    public Transform selfGun;
    public Transform ball;
    public ForceMode forceMode;
    public float rate = 10;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
        
        Vector3 vec = ball.rigidbody.velocity;
        if (vec.x > 50) vec.x = 50;
        if (vec.x < -50) vec.x = -50;
        if (vec.z > 50) vec.z = 50;
        if (vec.z < -50) vec.z = -50;
        ball.rigidbody.velocity = vec;
       // if (ball.position.z <= 0)
        {
            if (Input.GetMouseButtonUp(0))
            {
                Vector3 direction = ball.position - selfGun.position;
                direction.y = 0;
                //Debug.Log("1:" + direction);
                ball.rigidbody.AddForce(direction * rate, forceMode);
            }
        }
      //  Debug.Log("1:" + ball.rigidbody.velocity);
	}
}