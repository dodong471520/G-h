//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using System;

//public class NetworkController : MonoBehaviour
//{
//    void Start()
//    {
//        this.rigidbody.useGravity = false;
//    }
//    void Update()
//    {
//        NetworkListen();
//    }
//    private void NetworkListen()
//    {
//        this.rigidbody.useGravity = false;
//        this.rigidbody.angularVelocity = Vector3.zero;

//        if (SelfInfo.GamePlayerInfo.ContainsKey(this.name) && SelfInfo.GamePlayerInfo[this.name].que.Count > 0)
//        {
//            long networktime = (BaseInfo.NetWorkTime + BaseInfo.DelayBackTime + (long)((Time.time - BaseInfo.GameTime) * 1000f));
//            int i;
//            for (i = 0; i < SelfInfo.GamePlayerInfo[this.name].que.Count; i++)
//            {
//                if (networktime <= long.Parse((string)SelfInfo.GamePlayerInfo[this.name].que[i].data[18]))
//                {
//                    break;
//                }
//            }
//            if (i == SelfInfo.GamePlayerInfo[this.name].que.Count)
//                i--;

//            MessageElement message_item = SelfInfo.GamePlayerInfo[this.name].que[i];
//            MessageElement message_item2;
//            if (i > 0)
//            {
//                message_item2 = SelfInfo.GamePlayerInfo[this.name].que[i - 1];
//                SelfInfo.GamePlayerInfo[this.name].que.RemoveRange(0, i - 1);
//            }
//            else
//            {
//                message_item2 = SelfInfo.GamePlayerInfo[this.name].que[i];
//            }

//            Vector3 Nowvelocity = new Vector3(float.Parse((string)message_item.data[11]), float.Parse((string)message_item.data[12]), float.Parse((string)message_item.data[13]));
//            Vector3 Nowvelocity2 = new Vector3(float.Parse((string)message_item2.data[11]), float.Parse((string)message_item2.data[12]), float.Parse((string)message_item2.data[13]));
//            Vector3 NowVelocityA = new Vector3(float.Parse((string)message_item.data[14]), float.Parse((string)message_item.data[15]), float.Parse((string)message_item.data[16]));
//            Vector3 NowVelocityA2 = new Vector3(float.Parse((string)message_item2.data[14]), float.Parse((string)message_item2.data[15]), float.Parse((string)message_item2.data[16]));
//            Quaternion Nowrotation = new Quaternion(float.Parse((string)message_item.data[7]), float.Parse((string)message_item.data[8]), float.Parse((string)message_item.data[9]), float.Parse((string)message_item.data[10]));
//            Quaternion Nowrotation2 = new Quaternion(float.Parse((string)message_item2.data[7]), float.Parse((string)message_item2.data[8]), float.Parse((string)message_item2.data[9]), float.Parse((string)message_item2.data[10]));
//            Vector3 Nowposition = new Vector3(float.Parse((string)message_item.data[4]), float.Parse((string)message_item.data[5]), float.Parse((string)message_item.data[6]));
//            Vector3 Nowposition2 = new Vector3(float.Parse((string)message_item2.data[4]), float.Parse((string)message_item2.data[5]), float.Parse((string)message_item2.data[6]));

//            //~ Debug.Log(networktime - long.Parse((string)message_item.data[18]));
//            long TimeLimitExceed = 500;
//            //忽略大于固定延迟时间
//            if (networktime - long.Parse((string)message_item.data[18]) > TimeLimitExceed)
//            {
//                //~ Debug.Log(networktime - long.Parse((string)message_item.data[18]));
//                return;
//            }

//            //求新的位置点
//            float tmptime = 0f;
//            float delaytime = 0.1f;
//            if (Nowvelocity.magnitude != 0)
//                tmptime += (Nowposition - Nowposition2).magnitude / Nowvelocity2.magnitude;

//            //求移动中间点
//            float AddSpeedTime = (float)(networktime - long.Parse((string)message_item.data[18])) / 1000f;

//            float DisTime = (float)(long.Parse((string)message_item.data[18]) - long.Parse((string)message_item2.data[18])) / 1000f;
//            Vector3 VelocityA = NowVelocityA;
//            Vector3 NewPosition = Nowposition + Nowvelocity * AddSpeedTime + VelocityA * AddSpeedTime * AddSpeedTime / 2f + (Nowvelocity + AddSpeedTime * VelocityA) * delaytime;
//            Vector3 NewPosition1 = Nowposition + Nowvelocity * AddSpeedTime + VelocityA * AddSpeedTime * AddSpeedTime / 2f;

//            if ((NewPosition1 - this.transform.position).magnitude > 10f)
//            {
//                Debug.Log("Distance is larger than 10 meters!");
//                this.rigidbody.velocity = (Nowvelocity + AddSpeedTime * VelocityA) * delaytime;
//                this.rigidbody.angularVelocity = Vector3.zero;
//                this.transform.rotation = Nowrotation;
//                this.transform.position = NewPosition1;
//                return;
//            }
//            //~ Debug.DrawLine( NewPosition , NewPosition + Vector3.up*20f );
//            //~ this.transform.position = NewPosition;
//            //~ return;
//            //~ if( (NewPosition - this.rigidbody.position).magnitude > 10 )
//            //~ this.GetComponentInChildren<Collider>().isTrigger = true;
//            //~ else
//            //~ this.GetComponentInChildren<Collider>().isTrigger = false;

//            Vector3 NewVelocity;
//            //~ if( Vector3.Angle( ( Nowposition - NewPosition ).normalized , ( NewPosition - this.rigidbody.position ).normalized ) > 110f )
//            //~ {
//            //~ NewVelocity = ( Nowposition2 - Nowposition ).normalized * Nowvelocity.magnitude;
//            //~ dislen = ( (float)( networktime - long.Parse((string)message_item2.data[18]) ) + delaytime ) / ( (float)( long.Parse( (string)message_item.data[18] ) - long.Parse( (string)message_item2.data[18] ) ) );
//            //~ NewPosition = Vector3.Slerp( Nowposition2 , Nowposition , dislen );
//            //~ NewVelocity = ( NewPosition - this.rigidbody.position ).normalized * ( NewPosition - this.rigidbody.position ).magnitude / tmptime;
//            //~ Debug.Log("in back: "+ Vector3.Angle( ( Nowposition - NewPosition ).normalized , ( NewPosition - this.rigidbody.position ).normalized ) );
//            //~ }
//            //~ else
//            //~ {
//            //把新速度付给本物体
//            if (tmptime < 0.00001f)
//            {
//                NewVelocity = (NewPosition - this.rigidbody.position).normalized * Nowvelocity.magnitude;
//            }
//            else
//            {
//                NewVelocity = (NewPosition - this.rigidbody.position).normalized * (NewPosition - this.rigidbody.position).magnitude / tmptime;
//            }
//            this.rigidbody.velocity = NewVelocity;

//            //车辆方向改变旋转模拟
//            float len = 1f;
//            if ((NewPosition - this.rigidbody.position).magnitude > 0)
//                len = NewVelocity.magnitude * Time.deltaTime / (NewPosition - this.rigidbody.position).magnitude;
//            this.transform.rotation = Quaternion.Slerp(this.rigidbody.rotation, Nowrotation, len);
//        }
//    }
//}