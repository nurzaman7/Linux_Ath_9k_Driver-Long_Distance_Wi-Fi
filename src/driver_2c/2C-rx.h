#include<math.h>
#include<linux/string.h>

/*Externs from Tx and Main */
extern unsigned long int jiffy;
extern u8 mymacid[6];
extern int recvd_control; //added by zaved
extern int send_control; //added by zaved
extern int control_from; //added by zaved

extern struct sk_buff *globalskb,*globalskb1;
extern struct ieee80211_sub_if_data *globalsdata0,*globalsdata1;
extern struct ieee80211_channel *globalchan,*globalchan1;

#define MAX_DELAY_OFFSET 5


#define MacIDLE		0
#define SynTx		1
#define SynRx		2

void printSkb(struct sk_buff* skb,char* str);
	
/***************For 2C Implementation*****************************/
	
		extern int mode;
		extern int color;
		extern int slot_time;
		extern int total_slots;
		extern int slot_count;
		int delay_offset;
		extern int mac_state;
		extern int registered;
/*********************END******************************************/
extern int add_to_buffer(struct ieee80211_sub_if_data *sdata,struct sk_buff *skb, struct ieee80211_channel *chan );

struct tdma_control_info
	{
	__le16 rcv_frame_control;
	__le16 rcv_duration_id;
	u8 addr1[6];	
	u8 myparent[6];
	__le32 rcv_timestamp;
	__le16 rcv_slot_size;
	__le16 rcv_seq_ctrl;
	__le16 rcv_next_slot;
	__le16 rcv_nof_contention_slots;
	__le16 rcv_nof_slots;
	__le16 rcv_delay;
	};

struct tdma_data_info{
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	__le16 seq_ctrl;
};

struct eth_hdr{
	u8 da[6];
	u8 sa[6];
}__attribute__ ((packed));

struct sk_buff * recvData(struct sk_buff *skb)
{
	skb_reset_mac_header(skb);
	skb->ip_summed = CHECKSUM_NONE;
	if (skb->data[10]==0xff && skb->data[11]==0xff && skb->data[12]==0xff 
		&& skb->data[13]==0xff && skb->data[14]==0xff && skb->data[15]==0xff)
        {
                                        //Broadcast Packet.
               skb->pkt_type =PACKET_BROADCAST;

         }
       else
       	 {
              //Not a broadcast packet.
               skb->pkt_type= PACKET_HOST;
       	 }
                                //Determine Protocol from Ethernet protocol ID's 
      	// skb->protocol = ((((unsigned char)skb->data[59]))<<8) + ((unsigned char)skb->data[58]); 

	if(skb->data[24]==0x08 && skb->data[25]==0x06){
		skb->protocol=ETH_P_ARP;
	 }
	else{	
		skb->protocol=ETH_P_IP;
	 }      
// Skb_pull will remove 24 Byte TDMA Header// 
//and 2 Byte Protocl ID than it will remove //
	//4 Byte FCS from Tail///                
	
	skb_pull(skb,(24+2));	
	skb_trim(skb,skb->len-4);
	skb->transport_header=NULL;
	skb->network_header=NULL;
	skb_set_tail_pointer(skb, skb->len);
	memset(skb->cb,0,sizeof(skb->cb));
//	netif_receive_skb(skb);
	return skb; 	
}

struct sk_buff * recvCtrl(struct sk_buff *skb){
	struct tdma_control_info* ctrl_info;

	skb_trim(skb,skb->len-4);
	ctrl_info=(struct tdma_control_info *)(skb->data);
		
	if(ntohs(ctrl_info->rcv_frame_control)==0xf118){
		mac_state=SynRx;
		color=0;
	} else if(ntohs(ctrl_info->rcv_frame_control)==0xf110){
		color=1;
		mac_state=SynRx;
	}

	//printk("MyTimestamp:%d NewTimestamp:%d ",jiffy,ctrl_info->rcv_timestamp);

	jiffy=ctrl_info->rcv_timestamp+4;

	slot_time=ntohs(ctrl_info->rcv_slot_size);
	total_slots=ntohs(ctrl_info->rcv_nof_slots);

	slot_count=ntohs(ctrl_info->rcv_next_slot);

	//u8 addr1[6];	
	//u8 myparent[6];
	registered=1; //Which is YES	//This is a hack into the protocol [where node directly register after receiving control]
	return skb;
}


void forwardData(struct sk_buff * skb, struct net_device *dev)
	{
			
		//	printSkb(skb,"Skb Befor \n");
			struct eth_hdr *hdr;
			int hdrlen;
			unsigned char dummy[12]={0};
			hdr=(struct eth_hdr *)(dummy);
			memcpy(hdr->da, skb->data+10, ETH_ALEN);
			memcpy(hdr->sa, skb->data+16, ETH_ALEN);	
			skb_pull(skb,24);
			skb_trim(skb,skb->len-4);
			int head_need;
			head_need=sizeof(hdr)-skb_headroom(skb);
			if(head_need>0)
			{
				head_need=max_t(int,0,head_need);
				pskb_expand_head(skb,head_need,0,GFP_ATOMIC);
			}
			hdrlen=sizeof(struct eth_hdr);
		
			memcpy(skb_push(skb,hdrlen),hdr,hdrlen);
			
		//	printSkb(skb,"Skb After \n");
			if(globalsdata1 && dev->name[4]=='0' ){
			//	printk(KERN_INFO "Forwarding Packt through Wlan1\n");
				add_to_buffer(globalsdata1,skb,globalchan1);
			}
			else if(globalsdata0 && dev->name[4]=='1' ){
			//	printk(KERN_INFO "Forwarding Packt through Wlan0\n");
				add_to_buffer(globalsdata0,skb,globalchan);
			}
			


	}
