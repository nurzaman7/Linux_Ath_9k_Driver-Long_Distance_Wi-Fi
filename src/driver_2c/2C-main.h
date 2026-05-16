#include<linux/ktime.h>
#include<linux/hrtimer.h>
#include<linux/time.h>
#define MS_TO_NS(x)     (x * 1E6L)
#define MIS_TO_NS(x)     (x * 1E3L)

#define FIRST_ROUND	-1

#define ROOT 		0
#define RELAY		1
#define LEAF 		2

#define RED 		0
#define GREEN 		1

#define CONTROL 	0
#define NODE_JOIN 	1
#define DATA		2

#define MacIDLE		0
#define SynTx		1
#define SynRx		2

#define YES		1
#define NO		0
extern int sendData();
extern int sendCtrl();
extern int startSendTimer(int val);

/***************For 2C Implementation*****************************/
	
		int mode=LEAF;           // Mode of the Node
		int color=GREEN;            // Current Color for 2C
		int slot_time=4;	    // SLot Size = Slot_time X Timer Resolution
		int total_slots=100;        // Total Number of Slots in the Super Frame 
		int slot_count=FIRST_ROUND;           // Current Slot Number in the Super Frame
		unsigned long int jiffy=0L; // The timer value at any instant
		int mytime=0;               // Available Time in slot or Remaining TIme
		int guard_time=1000;             //Guard Time for a Slot
		int level=1;
		int mac_state=MacIDLE;
		int registered=YES;	// as it is root node it is alread registered
		int recvd_control=0;
		int send_control=0;
		int control_from=-1;
/*********************END******************************************/


///////////////////////////////////////////////////////////////
static struct hrtimer test_timer;


int testClock();

int testClock(){
        ktime_t ktime;
	int ret=0;
        unsigned long delay_in_ns=1000L;
        ktime=ktime_set(0,MIS_TO_NS(delay_in_ns));
        hrtimer_init(&test_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
        test_timer.function=&testClock;
        hrtimer_start(&test_timer,ktime,HRTIMER_MODE_REL);
	if(jiffy%slot_time==0){

		if(slot_count>=total_slots || slot_count==FIRST_ROUND){
			if(mode==ROOT){
				slot_count=0;
				mac_state=SynTx;
				color=GREEN;
				send_control=0;
				recvd_control=0;
				ret=sendCtrl();
			}else{				
				slot_count=FIRST_ROUND;
				mac_state=SynRx;
				color=RED;
				recvd_control=0;
			}	
			
		} else{
			/*Switching the Mac State*/
			if(mac_state==SynTx){
				color=RED;
				mac_state=SynRx;	
			//printk(KERN_INFO "MAC state is SynTx @ Jiffy = %ld",jiffy);		
			}else if(mac_state==SynRx){
				color=GREEN;
				mac_state=SynTx;
			//printk(KERN_INFO "MAC state is SynRx @ Jiffy = %ld",jiffy);		
			}
			/*          *		*/
			
			slot_count++; //incrementing the slot counter
			//printk(KERN_INFO "[%d]in this slot SynTx\n",slot_count);
			if(mac_state==SynTx){
				//printk(KERN_INFO "[%d]in this slot SynTx %d",slot_count,jiffy);
				if(!registered){
					if(recvd_control){
						//Here we send Node join
					}
					slot_count=FIRST_ROUND;
				} else{
					if(send_control){
						send_control=0;
						recvd_control=0;
						ret=sendCtrl(); //intermediate node sending control
					//	printk(KERN_INFO "Trying to send Control to wlan1\n");
						
					} else{
						//Here we send Data
						mytime=slot_time*1000;  
						ret=sendData();
					//	printk(KERN_INFO "[%d]This slot for DATA",slot_count);
					}
				}
			} else if(mac_state == SynRx){
				//Start receving data
				//printk(KERN_INFO "[%d]in this slot SynRx %d",slot_count,jiffy);
			}
			
		}

	} else{
		
	}
	jiffy=(jiffy+1)%4294967295;
        		return HRTIMER_NORESTART;

}

////////////////////////////////////////////////////////////////////////////////////////////
