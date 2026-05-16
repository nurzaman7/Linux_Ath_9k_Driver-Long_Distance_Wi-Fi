#include<linux/ktime.h>
#include<linux/hrtimer.h>
#include<linux/time.h>

#include<linux/module.h>      // for all modules 
#include<linux/init.h>        // for entry/exit macros 
#include<linux/kernel.h>      // for printk priority macros 
#include<asm/current.h>       // process information, just for fun 
#include<linux/sched.h>       // for "struct task_struct"
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>
#include <linux/syscalls.h>
#include <linux/delay.h>

#define QUEUE_SIZE 20
#define MS_TO_NS(x)     (x * 1E6L)
#define MIS_TO_NS(x)     (x * 1E3L)

#define ROOT 		0
#define RELAY		1
#define LEAF 		2

#define RED 0
#define GREEN 1


#define MacIDLE		0
#define SynTx		1
#define SynRx		2

int queue_len=0;
int rx_pkt=0;
int startSendTimer(int val);
int my_turn=0;

extern int mac_state;

struct sk_buff *globalskb=0,*globalskb1=0;
struct ieee80211_sub_if_data *globalsdata0=0,*globalsdata1=0;
struct ieee80211_channel *globalchan=0,*globalchan1=0;
u8 mymacid[6];
int add_to_buffer(struct ieee80211_sub_if_data *sdata,struct sk_buff *skb, struct ieee80211_channel *chan );
void printskb(struct sk_buff* skb, char* place);
	void printskb(struct sk_buff* skb, char* place)
		{
		int i;
		printk(KERN_INFO "%s-- ",place);
		for(i=0;i<=skb->len;i++)
		printk("%x-",skb->data[i]);

		}

/***************For 2C Implementation*****************************/
	
		extern int mode;
		extern int color;
		extern int slot_time;
		extern int total_slots;
		extern int slot_count;
		extern int delay_offset;
		extern int mytime;
		extern unsigned long int jiffy;
		extern int guard_time;
		extern int control_from;
/*********************END******************************************/


/**********************TDMA QUEUE*******************************/
struct tdma_queue {
	struct sk_buff *tdma_data;
	struct ieee80211_sub_if_data *tdmasdata;
	struct ieee80211_channel *tdmachan;
	struct tdma_queue *next;
};
/************************TDMA QUEUE END**************************/

/*************************TDMA DATA HEADER********************/

struct tdma_data_hdr{
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	__le16 seq_ctrl;
} __attribute__ ((packed));

/************************TDMA DATA HEADER END**********************/

/*******************TDMA CONTROL HEADER**************************/
struct tdma_control_hdr{
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	__le32 timestamp;
	__le16 slot_size;
	__le16 seq_ctrl;
	__le16 next_slot;
	__le16 nof_contention_slots;
	__le16 nof_slots;
	
} __attribute__ ((packed));
/******************TDMA CONTROL HEADER ENDS*********************/


/*****************ADDING PACKETS TO QUEUE**********************/
struct tdma_queue *tdmahead=0;
struct tdma_queue *tdmatail=0;

int add_to_buffer(struct ieee80211_sub_if_data *sdata,struct sk_buff *skb, struct ieee80211_channel *chan )
{
	if(queue_len<QUEUE_SIZE)
	{
		struct tdma_queue *tdma_entry=kmalloc(sizeof(struct tdma_queue),GFP_ATOMIC);	
		if(tdmahead==0)
		{
			tdmahead=tdma_entry;
			tdmatail=tdma_entry;
		}
		else
		{
			tdmatail->next=tdma_entry;
			tdmatail=tdma_entry;
		}
		tdma_entry->tdma_data=skb;
		tdma_entry->tdmasdata=sdata;
		tdma_entry->tdmachan=chan;
		tdma_entry->next=0;
		queue_len++;
	}
	else
	{
		dev_kfree_skb(skb);
	}
	return 0;
}
/**********************ADDING PACKETS TO QUEUE ENDS************************/

/*******************ADD TDMA DATA HEADER*********************************/

struct sk_buff * tdma_add_data_header(struct sk_buff *skb);

struct sk_buff * tdma_add_data_header(struct sk_buff *skb){
	struct tdma_data_hdr *hdr;
	int hdrlen;
	unsigned char dummy[24]={0};
	hdr=(struct tdma_data_hdr *)(dummy);

	hdr->frame_control=htons(0xf010);
	hdr->duration_id=0x0000;
	hdr->addr1[0]=0xff;
	hdr->addr1[1]=0xff;
 	hdr->addr1[2]=0xff;
 	hdr->addr1[3]=0xff;
	hdr->addr1[4]=0xff;
	hdr->addr1[5]=0xff;
	memcpy(hdr->addr2, skb->data, ETH_ALEN);
	memcpy(hdr->addr3, skb->data+ ETH_ALEN, ETH_ALEN);	
	memcpy(mymacid, skb->data+ETH_ALEN, ETH_ALEN);
	int head_need;
	head_need=sizeof(hdr)-skb_headroom(skb);
	if(head_need>0){
		head_need=max_t(int,0,head_need);
		pskb_expand_head(skb,head_need,0,GFP_ATOMIC);
	}
	hdrlen=sizeof(struct tdma_data_hdr);
	skb_pull(skb,12);
	memcpy(skb_push(skb,hdrlen),hdr,hdrlen);
	

	return skb;
}

/*****************ADDING DATA HEADER ENDS****************/

/****************** ADDING CONTROL HEADER*****************/
struct sk_buff * tdma_add_control_header(struct sk_buff *skb);

struct sk_buff * tdma_add_control_header(struct sk_buff *skb){
	
	struct tdma_control_hdr *hdr;
	int hdrlen;

	unsigned char dummy[30]={0};
	hdr=(struct tdma_control_hdr *)(dummy);
	if(color==0)
	hdr->frame_control=htons(0xf110);
	else
	hdr->frame_control=htons(0xf118);
	
	hdr->duration_id=0x0000;
	hdr->addr1[0]=0xff;
	hdr->addr1[1]=0xff;
 	hdr->addr1[2]=0xff;
 	hdr->addr1[3]=0xff;
	hdr->addr1[4]=0xff;
	hdr->addr1[5]=0xff;

	hdr->addr2[0]=0xff;
	hdr->addr2[1]=0xff;
 	hdr->addr2[2]=0xff;
 	hdr->addr2[3]=0xff;
	hdr->addr2[4]=0xff;
	hdr->addr2[5]=0xff;
	memcpy(hdr->addr2, skb->data+ETH_ALEN, ETH_ALEN);
	hdr->timestamp=jiffy;
	hdr->slot_size=htons(slot_time);//htons(0x0028);
	hdr->seq_ctrl=0x00;
	hdr->next_slot=ntohs(slot_count);
	hdr->nof_contention_slots=0x01;
	hdr->nof_slots=htons(total_slots);  //htons(0x0064);
	hdrlen=sizeof(struct tdma_control_hdr);
	if((skb->len)<hdrlen){
		int need_size=hdrlen-(skb->len);
		int head_need=need_size-skb_headroom(skb);
		if(head_need>0){
			head_need=max_t(int,0,head_need);
			pskb_expand_head(skb,head_need,0,GFP_ATOMIC);
		}
		skb_pull(skb,skb->len);
		memcpy(skb_push(skb,hdrlen),hdr,hdrlen);
		
	}
	else{
	
	skb_pull(skb,skb->len);
	memcpy(skb_push(skb,hdrlen),hdr,hdrlen);
	int remove_size=(skb->len)-hdrlen;
	skb_trim(skb,(skb->len)-remove_size);
	}

	return skb;
	
}
/***************ADDING CONTROL HEADER ENDS*********************/


int dataTime(int size){
			return (size*8/11);
		      }

/******************DATA SENDING FUNCTION***************/
int sendData(){
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_channel *chan;
	struct tdma_queue *tmp;
	rcu_read_lock();
	
	if(tdmahead){
			struct sk_buff *skb;
			skb=tdmahead->tdma_data;
			sdata=tdmahead->tdmasdata;
			chan=tdmahead->tdmachan;
			int pkt_time=dataTime(skb->len);

			//if(pkt_time<200) pkt_time=200;

			if(pkt_time<mytime-guard_time){
			
			if(tdmahead==tdmatail)
			{
				kfree(tdmahead);
				tdmahead=0;
				tdmatail=0;
			
			}
			else{
				tmp=tdmahead;
				tdmahead=tdmahead->next;
				kfree(tmp);
			}
			if(queue_len>0){
				queue_len--;
			}
			
			skb=tdma_add_data_header(skb);
			ieee80211_xmit(sdata,skb,chan->band);
			rx_pkt++;
		//	printk(KERN_INFO"NUmber of Packet sent= %d and Current Queue Len= %d \n", rx_pkt,queue_len);		
			mytime-=pkt_time;
			startSendTimer(pkt_time);
			}
		//skb=0;
	}
	else{
		/*if(mytime>1000){
			startSendTimer(1000);
			mytime-=1000;
		}*/
	}
	rcu_read_unlock();
	
	return 0;
}
/********************DATA SENDING FUNCTION ENDS*************************/

/*********************CONTROL SENDING FUNCTION *************/

int sendCtrl(){
	struct ieee80211_sub_if_data *sdata0;
	struct ieee80211_sub_if_data *sdata1;
	struct sk_buff *skb,*skb1;
	struct ieee80211_channel *chan,*chan1;

// Check for sdata0 and skb0 to avlblty else send a new raw pkt////////

	if(mode==0){ //The root node
		if(globalskb && globalsdata0){	
			skb=skb_copy(globalskb,GFP_KERNEL);
		//	printskb(skb,"skb0");
	
			sdata0=kmalloc(sizeof(struct ieee80211_sub_if_data),GFP_ATOMIC);
			memcpy(sdata0,globalsdata0,sizeof(struct ieee80211_sub_if_data));
			
			chan=kmalloc(sizeof(struct ieee80211_channel),GFP_ATOMIC);
			memcpy(chan,globalchan,sizeof(struct ieee80211_channel));
			
			skb=tdma_add_control_header(skb);
			
			ieee80211_xmit(sdata0,skb,chan->band);
			
			skb=0;
		rcu_read_unlock();
		} /*else{
			
               		 int retval=1;
               		 char * envp[] = { NULL };
              		 char* argv[]={"/bin/sendraw", NULL};//sendraw will send pkt through wlan0
               		 retval=call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
		}*/

		
// Check for sdata1 and skb1 to avlblty else send a new raw pkt////////

		 if(globalskb1 && globalsdata1){
		
				skb1=skb_copy(globalskb1,GFP_KERNEL);
				//printskb(skb,"skb1");
				sdata1=kmalloc(sizeof(struct ieee80211_sub_if_data),GFP_ATOMIC);
				memcpy(sdata1,globalsdata1,sizeof(struct ieee80211_sub_if_data));
			
				chan1=kmalloc(sizeof(struct ieee80211_channel),GFP_ATOMIC);
				memcpy(chan1,globalchan1,sizeof(struct ieee80211_channel));
				skb1=tdma_add_control_header(skb1);
						
				ieee80211_xmit(sdata1,skb1,chan1->band);
			
				skb1=0;
			rcu_read_unlock();
		} /*else{
			int retval=1;
		        char * envp[] = { NULL };
		        char* argv[]={"/bin/sendwlan", NULL};//sendwlan will send one RAW pkt through wlan1
		        retval=call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
		}*/
	} else{
		if(control_from==0){
			if(globalskb1 && globalsdata1){
		
				skb1=skb_copy(globalskb1,GFP_KERNEL);
				//printskb(skb,"skb1");
				sdata1=kmalloc(sizeof(struct ieee80211_sub_if_data),GFP_ATOMIC);
				memcpy(sdata1,globalsdata1,sizeof(struct ieee80211_sub_if_data));
			
				chan1=kmalloc(sizeof(struct ieee80211_channel),GFP_ATOMIC);
				memcpy(chan1,globalchan1,sizeof(struct ieee80211_channel));
				skb1=tdma_add_control_header(skb1);
						
				ieee80211_xmit(sdata1,skb1,chan1->band);
			
				skb1=0;
			rcu_read_unlock();
			}
		}else if(control_from==1){
			if(globalskb && globalsdata0){	
			skb=skb_copy(globalskb,GFP_KERNEL);
		//	printskb(skb,"skb0");
	
			sdata0=kmalloc(sizeof(struct ieee80211_sub_if_data),GFP_ATOMIC);
			memcpy(sdata0,globalsdata0,sizeof(struct ieee80211_sub_if_data));
			
			chan=kmalloc(sizeof(struct ieee80211_channel),GFP_ATOMIC);
			memcpy(chan,globalchan,sizeof(struct ieee80211_channel));
			
			skb=tdma_add_control_header(skb);
			
			ieee80211_xmit(sdata0,skb,chan->band);
			
			skb=0;
			rcu_read_unlock();
			}
		}else{
					
		}
	}
	return 0;

}

/*******************CONTROL SENDING FUNCTION ENDS ****************/


static struct hrtimer send_timer;
int startSendTimer(int val){
	int clk=0;
	/*if(val<100)
		clk=1000;
	else*/

	clk=val;
	ktime_t ktime;
	ktime=ktime_set(0,clk*1000);
	hrtimer_init(&send_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
	if(mac_state==SynTx){
	send_timer.function=&sendData;
	hrtimer_start(&send_timer,ktime,HRTIMER_MODE_REL);
	}
        return HRTIMER_NORESTART;
}

