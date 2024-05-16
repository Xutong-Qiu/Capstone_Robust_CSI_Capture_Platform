#pragma NEXMON targetregion "patch"

#include <firmware_version.h>
#include <wrapper.h>
#include <structs.h>
#include <patcher.h>
#include <helper.h>
#include <capabilities.h>
#include <channels.h>

extern void prepend_ethernet_ipv4_udp_header(struct sk_buff *p);

#define WL_RSSI_ANT_MAX     4           /* max possible rx antennas */

// header of csi frame coming from ucode
struct d11csihdr {
#define CSIDATA_PER_CHUNK   64
    uint16 NexmonCSICfg;
    uint32 csi[CSIDATA_PER_CHUNK>>2];
    uint16 csiconf;
    uint16 start;
    uint8  src[6];
    uint16 seqcnt;
    uint16 chanspec;
} __attribute__((packed));

// original hardware header
struct d11rxhdr {
    uint16 RxFrameSize;                 /* 0x000 Set to 0x2 for CSI frames */
    uint16 NexmonExt;                   /* 0x002 */
    uint16 PhyRxStatus_0;               /* 0x004 PhyRxStatus 15:0 */
    uint16 PhyRxStatus_1;               /* 0x006 PhyRxStatus 31:16 */
    uint16 PhyRxStatus_2;               /* 0x008 PhyRxStatus 47:32 */
    uint16 PhyRxStatus_3;               /* 0x00a PhyRxStatus 63:48 */
    uint16 PhyRxStatus_4;               /* 0x00c PhyRxStatus 79:64 */
    uint16 PhyRxStatus_5;               /* 0x00e PhyRxStatus 95:80 */
    uint16 RxStatus1;                   /* 0x010 MAC Rx Status */
    uint16 RxStatus2;                   /* 0x012 extended MAC Rx status */
    uint16 RxTSFTime;                   /* 0x014 RxTSFTime time of first MAC symbol + M_PHY_PLCPRX_DLY */
    uint16 RxChan;                      /* 0x016 gain code, channel radio code, and phy type */
    uint16 RxFameSize_0;                /* size of rx-frame in fifo-0 in case frame is copied to fifo-1 */
    uint16 HdrConvSt;                   /* hdr conversion status. Copy of ihr(RCV_HDR_CTLSTS). */
    uint16 AvbRxTimeL;                  /* AVB RX timestamp low16 */
    uint16 AvbRxTimeH;                  /* AVB RX timestamp high16 */
    uint16 MuRate;                      /* MU rate info (bit3:0 MCS, bit6:4 NSTS) */
    uint16 Pad;
} __attribute__((packed));

// header or regular frame coming from ucode
struct nexmon_d11rxhdr {
    struct d11rxhdr rxhdr;              /* 0x000 d11rxhdr */
    uint8 SrcMac[6];                    /* 0x018 source mac address */
} __attribute__((packed));

// header after process_frame_hook
struct wlc_d11rxhdr {
    struct d11rxhdr rxhdr;              /* 0x000 d11rxhdr */
    uint32  tsf_l;                      /* 0x018 TSF_L reading */
    int8    rssi;                       /* 0x01c computed instantaneous rssi in BMAC */
    int8    rxpwr0;                     /* 0x01d obsoleted, place holder for legacy ROM code. use rxpwr[] */
    int8    rxpwr1;                     /* 0x01e obsoleted, place holder for legacy ROM code. use rxpwr[] */
    int8    do_rssi_ma;                 /* 0x01f do per-pkt sampling for per-antenna ma in HIGH */
    int8    rxpwr[WL_RSSI_ANT_MAX];     /* 0x020 rssi for supported antennas */
    int8    rssi_qdb;                   /* 0x024 qdB portion of the computed rssi */
    uint8   PAD[2];                     /* 0x025 extra padding to fill up RXE_RXHDR_EXTRA */
} __attribute__((packed));

struct csi_udp_frame {
    struct ethernet_ip_udp_header hdrs;
    uint16 kk1;
    int8 rssi;
    uint8 fc; //frame control
    uint8 SrcMac[6];
    uint16 seqCnt;
    uint16 csiconf;
    uint16 chanspec;
    uint16 chip;
    uint32 csi_values[];
} __attribute__((packed));

struct int14 {signed int val:14;} __attribute__((packed));

uint16 missing_csi_frames = 0;
uint16 inserted_csi_values = 0;
struct sk_buff *p_csi = 0;
struct sk_buff *normal_frame = 0;
int8 last_rssi = 0;
uint16 phystatus[6] = {0,0,0,0, 0, 0};

void
create_new_csi_frame(struct wl_info *wl, uint16 csiconf, int length)
{
    struct osl_info *osh = wl->wlc->osh;
    // create new csi udp frame
    p_csi = pkt_buf_get_skb(osh, sizeof(struct csi_udp_frame) + length);
    if (p_csi == 0) {
        return;
    }
    // fill header
    struct csi_udp_frame *udpfrm = (struct csi_udp_frame *) p_csi->data;
    // add magic bytes, csi config and chanspec to new udp frame
    udpfrm->kk1 = 0x1111;
    udpfrm->rssi = last_rssi;
    udpfrm->fc = 0;
    udpfrm->seqCnt = 0;
    udpfrm->csiconf = csiconf;
    udpfrm->chanspec = get_chanspec(wl->wlc);
    udpfrm->chip = NEXMON_CHIP;
}

void
process_frame_hook(struct sk_buff *p, struct wlc_d11rxhdr *wlc_rxhdr, struct wlc_hw_info *wlc_hw, int tsf_l)
{
    struct wl_info *wl = wlc_hw->wlc->wl;
     struct osl_info *osh = wlc_hw->wlc->osh;
     
        // wlc_rxhdr->tsf_l = tsf_l;
        // wlc_phy_rssi_compute(wlc_hw->band->pi, wlc_rxhdr);
        // last_rssi = wlc_rxhdr->rssi;
        // struct d11rxhdr  * rxh = &wlc_rxhdr->rxhdr;
        // memcpy(phystatus, &rxh->PhyRxStatus_0, sizeof(phystatus));
        //wlc_recv(wlc_hw->wlc, p);
         normal_frame = pkt_buf_get_skb(osh, p->len - 0x84);
            normal_frame->head = p->head;
            normal_frame->end = p->end;
            normal_frame->len = p->len - 0x84;
            normal_frame->fcseq = p->fcseq;
            normal_frame->flags = p->flags;
            normal_frame->next = p->next;
            normal_frame->link = p->link;
            for(uint i = 0x84; i < p->len; i++){
                ((char*)(normal_frame->data))[i-0x84] = ((char*)(p->data))[i];
            }
        // char init_values[] = {0x00, 0x00, 0x24, 0x00, 0x6f, 0x08, 0x00, 0x40, 0x80, 0xc6, 0x81, 0xa0, 0x00, 0x00, 0x00, 0x00,
        //                         0x12, 0x0c, 0x7c, 0x15, 0x40,0x01, 0xe5, 0xa9, 0x01, 0x00, 0x00, 0x10, 0x18, 0x03, 0x04, 0x00,
        //                         0x02, 0x00, 0x00, 0xCC};
        // memcpy(normal_frame->data+ p->len - 36, init_values, sizeof(init_values));
        wl->dev->chained->funcs->xmit(wl->dev, wl->dev->chained, normal_frame);
        pkt_buf_free_skb(osh, p, 0);
        //wlc_recv(wlc_hw->wlc, p);

    
}

__attribute__((at(0x1B2012, "", CHIP_VER_BCM4358, FW_VER_7_112_300_14)))
__attribute__((at(0x1C1C3E, "", CHIP_VER_BCM43455c0, FW_VER_7_45_189)))
__attribute__((at(0x2456E8, "", CHIP_VER_BCM4366c0, FW_VER_10_10_122_20)))
__attribute__((naked))
void
process_frame_prehook_off0x8(void)
{
    asm(
        "mov r2, r4\n"              // 2 bytes: move wlc_hw pointer to r2
        "ldr r0, [sp,0x8]\n"        // 2 bytes: load reference to p into r0
        "bl process_frame_hook\n"   // 4 bytes
        "nop\n"                     // 2 bytes: to overwrite existing instruction
        "nop\n"                     // 2 bytes: to overwrite existing instruction
        "nop\n"                     // 2 bytes: to overwrite existing instruction
        "nop\n"                     // 2 bytes: to overwrite existing instruction
        "nop\n"                     // 2 bytes: to overwrite existing instruction
        "nop\n"                     // 2 bytes: to overwrite existing instruction
    );
}


// Increase d11rxhdr size in initvals (found: below 0c 04 02 00)
__attribute__((at(0x1D4370, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
__attribute__((at(0x1F5768, "", CHIP_VER_BCM43455c0, FW_VER_7_45_189)))
__attribute__((at(0x1DF03C, "", CHIP_VER_BCM4358, FW_VER_7_112_300_14)))
__attribute__((at(0x28395C, "", CHIP_VER_BCM4366c0, FW_VER_10_10_122_20)))
GenericPatch1(initvals_rxhdr_len0, (RXE_RXHDR_LEN * 2));

__attribute__((at(0x1D4388, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
__attribute__((at(0x1F5778, "", CHIP_VER_BCM43455c0, FW_VER_7_45_189)))
__attribute__((at(0x1DF054, "", CHIP_VER_BCM4358, FW_VER_7_112_300_14)))
__attribute__((at(0x28396C, "", CHIP_VER_BCM4366c0, FW_VER_10_10_122_20)))
GenericPatch1(initvals_rxhdr_len1, (RXE_RXHDR_LEN * 2));

// Increase d11rxhdr size in wlc->hwrxhdr variable in wlc_bmac_attach
__attribute__((at(0x1F580C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
__attribute__((at(0x210F56, "", CHIP_VER_BCM43455c0, FW_VER_7_45_189)))
__attribute__((at(0x1F67F8, "", CHIP_VER_BCM4358, FW_VER_7_112_300_14)))
__attribute__((at(0x2F4328, "", CHIP_VER_BCM4366c0, FW_VER_10_10_122_20)))
GenericPatch1(hwrxoff, (RXE_RXHDR_LEN * 2) + RXE_RXHDR_EXTRA);

// Increase d11rxhdr size in wlc->hwrxoff_pktget variable in wlc_bmac_attach
__attribute__((at(0x1F5812, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
__attribute__((at(0x210F60, "", CHIP_VER_BCM43455c0, FW_VER_7_45_189)))
__attribute__((at(0x1F6802, "", CHIP_VER_BCM4358, FW_VER_7_112_300_14)))
__attribute__((at(0x2F4332, "", CHIP_VER_BCM4366c0, FW_VER_10_10_122_20)))
GenericPatch1(hwrxoff_pktget, (RXE_RXHDR_LEN * 2) + RXE_RXHDR_EXTRA + 2);


