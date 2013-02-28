#ifndef __DRBD_PROTOCOL_H
#define __DRBD_PROTOCOL_H

enum drbd_packet {
	/* receiver (data socket) */
	P_DATA		      = 0x00,
	P_DATA_REPLY	      = 0x01, /* Response to P_DATA_REQUEST */
	P_RS_DATA_REPLY	      = 0x02, /* Response to P_RS_DATA_REQUEST */
	P_BARRIER	      = 0x03,
	P_BITMAP	      = 0x04,
	P_BECOME_SYNC_TARGET  = 0x05,
	P_BECOME_SYNC_SOURCE  = 0x06,
	P_UNPLUG_REMOTE	      = 0x07, /* Used at various times to hint the peer */
	P_DATA_REQUEST	      = 0x08, /* Used to ask for a data block */
	P_RS_DATA_REQUEST     = 0x09, /* Used to ask for a data block for resync */
	P_SYNC_PARAM	      = 0x0a,
	P_PROTOCOL	      = 0x0b,
	P_UUIDS		      = 0x0c,
	P_SIZES		      = 0x0d,
	P_STATE		      = 0x0e,
	P_SYNC_UUID	      = 0x0f,
	P_AUTH_CHALLENGE      = 0x10,
	P_AUTH_RESPONSE	      = 0x11,
	P_STATE_CHG_REQ	      = 0x12,

	/* asender (meta socket */
	P_PING		      = 0x13,
	P_PING_ACK	      = 0x14,
	P_RECV_ACK	      = 0x15, /* Used in protocol B */
	P_WRITE_ACK	      = 0x16, /* Used in protocol C */
	P_RS_WRITE_ACK	      = 0x17, /* Is a P_WRITE_ACK, additionally call set_in_sync(). */
	P_SUPERSEDED	      = 0x18, /* Used in proto C, two-primaries conflict detection */
	P_NEG_ACK	      = 0x19, /* Sent if local disk is unusable */
	P_NEG_DREPLY	      = 0x1a, /* Local disk is broken... */
	P_NEG_RS_DREPLY	      = 0x1b, /* Local disk is broken... */
	P_BARRIER_ACK	      = 0x1c,
	P_STATE_CHG_REPLY     = 0x1d,

	/* "new" commands, no longer fitting into the ordering scheme above */

	P_OV_REQUEST	      = 0x1e, /* data socket */
	P_OV_REPLY	      = 0x1f,
	P_OV_RESULT	      = 0x20, /* meta socket */
	P_CSUM_RS_REQUEST     = 0x21, /* data socket */
	P_RS_IS_IN_SYNC	      = 0x22, /* meta socket */
	P_SYNC_PARAM89	      = 0x23, /* data socket, protocol version 89 replacement for P_SYNC_PARAM */
	P_COMPRESSED_BITMAP   = 0x24, /* compressed or otherwise encoded bitmap transfer */
	/* P_CKPT_FENCE_REQ      = 0x25, * currently reserved for protocol D */
	/* P_CKPT_DISABLE_REQ    = 0x26, * currently reserved for protocol D */
	P_DELAY_PROBE         = 0x27, /* is used on BOTH sockets */
	P_OUT_OF_SYNC         = 0x28, /* Mark as out of sync (Outrunning), data socket */
	P_RS_CANCEL           = 0x29, /* meta: Used to cancel RS_DATA_REQUEST packet by SyncSource */
	P_CONN_ST_CHG_REQ     = 0x2a, /* data sock: state change request */
	P_CONN_ST_CHG_REPLY   = 0x2b, /* meta sock: state change reply */
	P_RETRY_WRITE	      = 0x2c, /* Protocol C: retry conflicting write request */
	P_PROTOCOL_UPDATE     = 0x2d, /* data sock: is used in established connections */
	P_TWOPC_PREPARE       = 0x2e, /* data sock: prepare state change */
	P_TWOPC_ABORT         = 0x2f, /* data sock: abort state change */

	P_DAGTAG	      = 0x30, /* data sock: set the current dagtag */
	/* P_TRIM                = 0x31, Reserved */
	P_PEER_ACK            = 0x32, /* meta sock: tell which nodes have acked a request */
	P_PEERS_IN_SYNC       = 0x33, /* data sock: Mark area as in sync */

	P_UUIDS110	      = 0x34, /* data socket */
	P_PEER_DAGTAG         = 0x35, /* data socket, used to trigger reconciliation resync */
	P_CURRENT_UUID	      = 0x36, /* data socket */

	P_TWOPC_YES           = 0x37, /* meta sock: allow two-phase commit */
	P_TWOPC_NO            = 0x38, /* meta sock: reject two-phase commit */
	P_TWOPC_COMMIT        = 0x39, /* data sock: commit state change */
	P_TWOPC_RETRY         = 0x40, /* meta sock: retry two-phase commit */

	P_PRI_REACHABLE       = 0x41, /* The set of reachable primaries got smaller */

	P_MAY_IGNORE	      = 0x100, /* Flag to test if (cmd > P_MAY_IGNORE) ... */

	/* special command ids for handshake */

	P_INITIAL_META	      = 0xfff1, /* First Packet on the MetaSock */
	P_INITIAL_DATA	      = 0xfff2, /* First Packet on the Socket */

	P_CONNECTION_FEATURES = 0xfffe	/* FIXED for the next century! */
};

#ifndef __packed
#define __packed __attribute__((packed))
#endif

/* This is the layout for a packet on the wire.
 * The byteorder is the network byte order.
 *     (except block_id and barrier fields.
 *	these are pointers to local structs
 *	and have no relevance for the partner,
 *	which just echoes them as received.)
 *
 * NOTE that the payload starts at a long aligned offset,
 * regardless of 32 or 64 bit arch!
 */
struct p_header80 {
	u32	  magic;
	u16	  command;
	u16	  length;	/* bytes of data after this header */
} __packed;

/* Header for big packets, Used for data packets exceeding 64kB */
struct p_header95 {
	u16	  magic;	/* use DRBD_MAGIC_BIG here */
	u16	  command;
	u32	  length;
} __packed;

struct p_header100 {
	u32	  magic;
	u16	  volume;
	u16	  command;
	u32	  length;
	u32	  pad;
} __packed;

/* these defines must not be changed without changing the protocol version */
#define DP_HARDBARRIER	      1 /* no longer used */
#define DP_RW_SYNC	      2 /* equals REQ_SYNC    */
#define DP_MAY_SET_IN_SYNC    4
#define DP_UNPLUG             8 /* equals REQ_UNPLUG (compat) */
#define DP_FUA               16 /* equals REQ_FUA     */
#define DP_FLUSH             32 /* equals REQ_FLUSH   */
#define DP_DISCARD           64 /* equals REQ_DISCARD */
#define DP_SEND_RECEIVE_ACK 128 /* This is a proto B write request */
#define DP_SEND_WRITE_ACK   256 /* This is a proto C write request */

struct p_data {
	u64	    sector;    /* 64 bits sector number */
	u64	    block_id;  /* to identify the request in protocol B&C */
	u32	    seq_num;
	u32	    dp_flags;
} __packed;

/*
 * commands which share a struct:
 *  p_block_ack:
 *   P_RECV_ACK (proto B), P_WRITE_ACK (proto C),
 *   P_SUPERSEDED (proto C, two-primaries conflict detection)
 *  p_block_req:
 *   P_DATA_REQUEST, P_RS_DATA_REQUEST
 */
struct p_block_ack {
	u64	    sector;
	u64	    block_id;
	u32	    blksize;
	u32	    seq_num;
} __packed;

struct p_block_req {
	u64 sector;
	u64 block_id;
	u32 blksize;
	u32 pad;	/* to multiple of 8 Byte */
} __packed;

/*
 * commands with their own struct for additional fields:
 *   P_CONNECTION_FEATURES
 *   P_BARRIER
 *   P_BARRIER_ACK
 *   P_SYNC_PARAM
 *   ReportParams
 */

struct p_connection_features {
	u32 protocol_min;
	u32 feature_flags;
	u32 protocol_max;
	u32 sender_node_id;
	u32 receiver_node_id;

	/* should be more than enough for future enhancements
	 * for now, feature_flags and the reserved array shall be zero.
	 */

	u32 _pad;
	u64 reserved[6];
} __packed;

struct p_barrier {
	u32 barrier;	/* barrier number _handle_ only */
	u32 pad;	/* to multiple of 8 Byte */
} __packed;

struct p_barrier_ack {
	u32 barrier;
	u32 set_size;
} __packed;

struct p_rs_param {
	u32 resync_rate;

	      /* Since protocol version 88 and higher. */
	char verify_alg[0];
} __packed;

struct p_rs_param_89 {
	u32 resync_rate;
        /* protocol version 89: */
	char verify_alg[SHARED_SECRET_MAX];
	char csums_alg[SHARED_SECRET_MAX];
} __packed;

struct p_rs_param_95 {
	u32 resync_rate;
	char verify_alg[SHARED_SECRET_MAX];
	char csums_alg[SHARED_SECRET_MAX];
	u32 c_plan_ahead;
	u32 c_delay_target;
	u32 c_fill_target;
	u32 c_max_rate;
} __packed;

enum drbd_conn_flags {
	CF_DISCARD_MY_DATA = 1,
	CF_DRY_RUN = 2,
};

struct p_protocol {
	u32 protocol;
	u32 after_sb_0p;
	u32 after_sb_1p;
	u32 after_sb_2p;
	u32 conn_flags;
	u32 two_primaries;

              /* Since protocol version 87 and higher. */
	char integrity_alg[0];

} __packed;

struct p_uuids {
	u64 current_uuid;
	u64 bitmap_uuid;
	u64 history_uuids[HISTORY_UUIDS_V08];
	u64 dirty_bits;
	u64 uuid_flags;
} __packed;

struct p_uuids110 {
	u64 current_uuid;
	u64 dirty_bits;
	u64 uuid_flags;
	u64 offline_mask;
	u64 bitmap_uuids_mask; /* non zero bitmap UUIDS for these nodes */
	u64 other_uuids[0]; /* the first hweight(nr_bitmap_uuids) slots carry bitmap uuids.
			       The node with the lowest node_id first.
			       The remaining slots carry history uuids */
} __packed;

struct p_uuid {
	u64	    uuid;
} __packed;

struct p_sizes {
	u64	    d_size;  /* size of disk */
	u64	    u_size;  /* user requested size */
	u64	    c_size;  /* current exported size */
	u32	    max_bio_size;  /* Maximal size of a BIO */
	u16	    queue_order_type;  /* not yet implemented in DRBD*/
	u16	    dds_flags; /* use enum dds_flags here. */
} __packed;

struct p_state {
	u32	    state;
} __packed;

struct p_req_state {
	u32	    mask;
	u32	    val;
} __packed;

struct p_req_state_reply {
	u32	    retcode;
} __packed;

struct p_twopc_request {
	u32 tid;  /* transaction identifier */
	u32 initiator_node_id;  /* initiator of the transaction */
	u32 target_node_id;  /* target of the transaction (or -1) */
	u64 nodes_to_reach;
	u64 primary_nodes;
	u64 weak_nodes;
	u32 mask;
	u32 val;
} __packed;

struct p_twopc_reply {
	u32 tid;  /* transaction identifier */
	u32 initiator_node_id;  /* initiator of the transaction */
	u64 reachable_nodes;
	u64 primary_nodes;
	u64 weak_nodes;
} __packed;

struct p_drbd06_param {
	u64	  size;
	u32	  state;
	u32	  blksize;
	u32	  protocol;
	u32	  version;
	u32	  gen_cnt[5];
	u32	  bit_map_gen[5];
} __packed;

struct p_block_desc {
	u64 sector;
	u32 blksize;
	u32 pad;	/* to multiple of 8 Byte */
} __packed;

/* Valid values for the encoding field.
 * Bump proto version when changing this. */
enum drbd_bitmap_code {
	/* RLE_VLI_Bytes = 0,
	 * and other bit variants had been defined during
	 * algorithm evaluation. */
	RLE_VLI_Bits = 2,
};

struct p_compressed_bm {
	/* (encoding & 0x0f): actual encoding, see enum drbd_bitmap_code
	 * (encoding & 0x80): polarity (set/unset) of first runlength
	 * ((encoding >> 4) & 0x07): pad_bits, number of trailing zero bits
	 * used to pad up to head.length bytes
	 */
	u8 encoding;

	u8 code[0];
} __packed;

struct p_delay_probe93 {
	u32     seq_num; /* sequence number to match the two probe packets */
	u32     offset;  /* usecs the probe got sent after the reference time point */
} __packed;

struct p_dagtag {
	u64 dagtag;
} __packed;

struct p_peer_ack {
	u64 mask;
	u64 dagtag;
} __packed;

struct p_peer_block_desc {
	u64 sector;
	u64 mask;
	u32 size;
	u32 pad;	/* to multiple of 8 Byte */
} __packed;

struct p_peer_dagtag {
	u64 dagtag;
	u32 node_id;
} __packed;

struct p_pri_reachable {
	u64 primary_mask;
} __packed;

/*
 * Bitmap packets need to fit within a single page on the sender and receiver,
 * so we are limited to 4 KiB (and not to PAGE_SIZE, which can be bigger).
 */
#define DRBD_SOCKET_BUFFER_SIZE 4096

#endif  /* __DRBD_PROTOCOL_H */