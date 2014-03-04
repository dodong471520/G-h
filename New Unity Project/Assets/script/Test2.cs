using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class Test2 : MonoBehaviour
{
    public Transform cube1;
    public Transform cube2;
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.A))
        {
            cube1.rigidbody.AddForce(Vector3.right*100, ForceMode.Acceleration);
            cube2.rigidbody.velocity = Vector3.right;
        }
    }
}
