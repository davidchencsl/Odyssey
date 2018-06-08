#ifndef ABD_MAIN_H
#define ABD_MAIN_H

#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>
#include "city.h"
#include "hrd.h"
//-------------------------------------------
/* ----------SYSTEM------------------------ */
//-------------------------------------------
#define TOTAL_CORES 40
#define TOTAL_CORES_ (TOTAL_CORES - 1)
#define SOCKET_NUM 2
#define PHYSICAL_CORES_PER_SOCKET 10
#define PHYSICAL_CORE_DISTANCE 4 // distance between two physical cores of the same socket
#define VIRTUAL_CORES_PER_SOCKET 20
#define WORKER_HYPERTHREADING 1
#define MAX_SERVER_PORTS 1 // better not change that

#define WORKERS_PER_MACHINE 39
#define MACHINE_NUM 3
#define REM_MACH_NUM (MACHINE_NUM - 1) // NUmber of remote machines

#define WORKER_NUM (WORKERS_PER_MACHINE * MACHINE_NUM)

#define CACHE_SOCKET 0// (WORKERS_PER_MACHINE < 30 ? 0 : 1 )// socket where the cache is bind

#define ENABLE_MULTIPLE_SESSIONS 1

#define SESSION_BYTES 3 // session ids must fit in 3 bytes i.e.



#define ENABLE_CACHE_STATS 0
#define EXIT_ON_PRINT 0
#define PRINT_NUM 4
#define DUMP_STATS_2_FILE 0


/*-------------------------------------------------
-----------------DEBUGGING-------------------------
--------------------------------------------------*/

#define MEASURE_LATENCY 0
#define REMOTE_LATENCY_MARK 100 // mark a remote request for measurement by attaching this to the imm_data of the wr
#define USE_A_SINGLE_KEY 0
#define DISABLE_HYPERTHREADING 0 // do not shcedule two threads on the same core
#define DEFAULT_SL 0 //default service level



/*-------------------------------------------------
	-----------------TRACE-----------------
--------------------------------------------------*/

#define SKEW_EXPONENT_A 99 // representation divided by 100 (i.e. 99 means a = 0.99)
#define DISABLE_CACHE 0
#define LOAD_BALANCE 1 // Use a uniform access pattern


/*-------------------------------------------------
	-----------------MULTICAST-------------------------
--------------------------------------------------*/
// Multicast defines are not used in the ABD, but we keep them for possible extension
#define ENABLE_MULTICAST 0
#define MULTICAST_TESTING_ 0
#define MULTICAST_TESTING (ENABLE_MULTICAST == 1 ? MULTICAST_TESTING_ : 0)
#define MCAST_QPS MACHINE_NUM


#define MCAST_QP_NUM 2
#define PREP_MCAST_QP 0
#define COM_MCAST_QP 1 //
#define MCAST_GROUPS_NUM 2

// ------COMMON-------------------
#define MAX_BCAST_BATCH (1) //how many broadcasts can fit in a batch
#define MESSAGES_IN_BCAST (REM_MACH_NUM)
#define MESSAGES_IN_BCAST_BATCH (MAX_BCAST_BATCH * MESSAGES_IN_BCAST) //must be smaller than the q_depth

/* --------------------------------------------------------------------------------
 * -----------------------------ABD------------------------------------------------
 * --------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------*/

// CORE CONFIGURATION
#define SESSIONS_PER_THREAD 80
#define ENABLE_LIN 1
#define R_CREDITS 2
#define MAX_R_COALESCE 40
#define W_CREDITS 8
#define MAX_W_COALESCE 15
#define ENABLE_ASSERTIONS 0
#define USE_QUORUM 1

#define ENABLE_ADAPTIVE_INLINING 0 // This did not help
#define MIN_SS_BATCH 127// The minimum SS batch
#define ENABLE_STAT_COUNTING 1
#define MAXIMUM_INLINE_SIZE 188



#define QP_NUM 4
#define R_QP_ID 0
#define R_REP_QP_ID 1
#define W_QP_ID 2
#define ACK_QP_ID 3
#define FC_QP_ID 4 // NOT USED!



#define QUORUM_NUM ((MACHINE_NUM / 2) + 1)
#define QUORUM_OF_ACKS (USE_QUORUM == 1 ? (QUORUM_NUM - 1): REM_MACH_NUM)

#define TS_TUPLE_SIZE (5) // version and m_id consist the Timestamp tuple


// READS

#define R_SIZE (TRUE_KEY_SIZE + TS_TUPLE_SIZE + 1)// key+ version + m_id + opcode
#define R_MES_HEADER (10) // local id + coalesce num + m_id
#define R_MES_SIZE (R_MES_HEADER + (R_SIZE * MAX_R_COALESCE))
#define R_RECV_SIZE (GRH_SIZE + R_MES_SIZE)
#define MAX_RECV_R_WRS (R_CREDITS * REM_MACH_NUM)
#define MAX_INCOMING_R (MAX_RECV_R_WRS * MAX_R_COALESCE)
#define MAX_R_WRS (MESSAGES_IN_BCAST_BATCH)
#define R_SEND_SIZE (R_MES_SIZE)
#define R_ENABLE_INLINING ((R_SEND_SIZE > MAXIMUM_INLINE_SIZE) ?  0 : 1)

// READ REPLIES
#define MAX_R_REP_COALESCE MAX_R_COALESCE
#define R_REP_MES_HEADER (8 + 3) //l_id, coalesce_num, m_id, opcode // and credits
#define R_REP_SIZE (TS_TUPLE_SIZE + VALUE_SIZE + 1)
#define R_REP_LIN_PUT_SIZE (TS_TUPLE_SIZE + 1)
#define R_REP_SMALL_SIZE (1)
#define R_REP_SEND_SIZE (R_REP_MES_HEADER + (MAX_R_REP_COALESCE * R_REP_SIZE))
#define R_REP_RECV_SIZE (GRH_SIZE + R_REP_SEND_SIZE)
#define MAX_RECV_R_REP_WRS (REM_MACH_NUM * R_CREDITS)
#define MAX_R_REP_WRS (R_CREDITS * REM_MACH_NUM * (CEILING(MAX_R_COALESCE, MAX_R_REP_COALESCE)))

#define R_REP_ENABLE_INLINING ((R_REP_SEND_SIZE > MAXIMUM_INLINE_SIZE) ?  0 : 1)
#define R_REP_FIFO_SIZE (MAX_INCOMING_R)
#define READ_INFO_SIZE (3 + TS_TUPLE_SIZE + TRUE_KEY_SIZE + VALUE_SIZE)

// Writes

#define MAX_RECV_W_WRS (W_CREDITS * REM_MACH_NUM)
#define MAX_W_WRS (MESSAGES_IN_BCAST_BATCH)
#define MAX_INCOMING_W (MAX_RECV_W_WRS * MAX_W_COALESCE)

#define WRITE_HEADER (TRUE_KEY_SIZE + TS_TUPLE_SIZE + 2) // opcode + val_len
#define W_SIZE (VALUE_SIZE + WRITE_HEADER)
#define W_MES_HEADER (8 + 1 +1) // local id + m_id+ w_num
#define W_MES_SIZE (W_MES_HEADER + (W_SIZE * MAX_W_COALESCE))
#define W_RECV_SIZE (GRH_SIZE + W_MES_SIZE)
#define W_ENABLE_INLINING ((W_MES_SIZE > MAXIMUM_INLINE_SIZE) ?  0 : 1)

// Acks
#define MAX_RECV_ACK_WRS (REM_MACH_NUM * W_CREDITS)
#define MAX_ACK_WRS (MACHINE_NUM)
#define ACK_SIZE 14
#define ACK_RECV_SIZE (GRH_SIZE + (ACK_SIZE))

#define VC_NUM 4
#define R_VC 0
#define W_VC 1



// BUFFER SIZES
#define R_BUF_SLOTS (REM_MACH_NUM * R_CREDITS)
#define R_BUF_SIZE (R_RECV_SIZE * R_BUF_SLOTS)

#define R_REP_BUF_SLOTS (REM_MACH_NUM * R_CREDITS)
#define R_REP_BUF_SIZE (R_REP_RECV_SIZE * R_REP_BUF_SLOTS)

#define W_BUF_SLOTS (REM_MACH_NUM * W_CREDITS)
#define W_BUF_SIZE (W_RECV_SIZE * W_BUF_SLOTS)

#define ACK_BUF_SLOTS (REM_MACH_NUM * W_CREDITS)
#define ACK_BUF_SIZE (ACK_RECV_SIZE * ACK_BUF_SLOTS)

#define TOTAL_BUF_SIZE (R_BUF_SIZE + R_REP_BUF_SIZE + W_BUF_SIZE + ACK_BUF_SIZE)
#define TOTAL_BUF_SLOTS (R_BUF_SLOTS + R_REP_BUF_SLOTS + W_BUF_SLOTS + ACK_BUF_SLOTS)

#define PENDING_READS (SESSIONS_PER_THREAD + 1)
#define EXTRA_WRITE_SLOTS 50 // to accommodate reads that become writes
#define PENDING_WRITES (SESSIONS_PER_THREAD + 1)
#define W_FIFO_SIZE (PENDING_WRITES)
#define R_FIFO_SIZE (PENDING_READS)

#define W_BCAST_SS_BATCH MAX((MIN_SS_BATCH / (REM_MACH_NUM)), (MESSAGES_IN_BCAST_BATCH + 1))
#define R_BCAST_SS_BATCH MAX((MIN_SS_BATCH / (REM_MACH_NUM)), (MESSAGES_IN_BCAST_BATCH + 2))
#define R_REP_SS_BATCH MAX(MIN_SS_BATCH, (MAX_R_REP_WRS + 1))
#define ACK_SS_BATCH MAX(MIN_SS_BATCH, (MAX_ACK_WRS + 2))


//  Receive
#define RECV_ACK_Q_DEPTH (MAX_RECV_ACK_WRS + 3)
#define RECV_W_Q_DEPTH  (MAX_RECV_W_WRS + 3) //
#define RECV_R_Q_DEPTH (MAX_RECV_R_WRS + 3) //
#define RECV_R_REP_Q_DEPTH (MAX_RECV_R_REP_WRS + 3)

// Send
#define SEND_ACK_Q_DEPTH ((2 * ACK_SS_BATCH) + 3)
#define SEND_W_Q_DEPTH ((2 * W_BCAST_SS_BATCH * REM_MACH_NUM) + 3) // Do not question or doubt the +3!!
#define SEND_R_Q_DEPTH ((2 * R_BCAST_SS_BATCH * REM_MACH_NUM) + 3) //
#define SEND_R_REP_Q_DEPTH ((2 * R_REP_SS_BATCH) + 3)


// DEBUG
#define DEBUG_WRITES 0
#define DEBUG_ACKS 0
#define DEBUG_READS 0
#define DEBUG_TS 0
#define CHECK_DBG_COUNTERS 0
#define VERBOSE_DBG_COUNTER 0
#define DEBUG_SS_BATCH 0
#define R_TO_W_DEBUG 0





//LATENCY Measurment
#define MAX_LATENCY 400 //in us
#define LATENCY_BUCKETS 200 //latency accuracy

/* SHM key for the 1st request region created by master. ++ for other RRs.*/
#define MASTER_SHM_KEY 24


//Defines for parsing the trace
#define _200_K 200000
#define MAX_TRACE_SIZE _200_K
#define FEED_FROM_TRACE 0
#define TRACE_SIZE K_128
#define NOP 0
#define HOT_WRITE 1
#define HOT_READ 2
#define REMOTE_WRITE 3
#define REMOTE_READ 4
#define LOCAL_WRITE 5
#define LOCAL_READ 6



#define IS_READ(X)  ((X) == HOT_READ || (X) == LOCAL_READ || (X) == REMOTE_READ  ? 1 : 0)
#define IS_WRITE(X)  ((X) == HOT_WRITE || (X) == LOCAL_WRITE || (X) == REMOTE_WRITE  ? 1 : 0)



struct trace_command {
	uint8_t  opcode;
	uint8_t  home_machine_id;
//	uint8_t  home_worker_id;
	uint32_t key_id;
	uint128 key_hash;
};

struct trace_command_uni {
  uint8_t  opcode;
  uint8_t key_hash[8];
};

/* ah pointer and qpn are accessed together in the critical path
   so we are putting them in the same cache line */
struct remote_qp {
	struct ibv_ah *ah;
	int qpn;
	// no padding needed- false sharing is not an issue, only fragmentation
};

/*
 *  SENT means the message has been sent
 *  READY means all acks have been gathered // OR a commit has been received
 * */

#define INVALID 0
#define VALID 1
#define SENT 2
#define READY 3

// Possible write sources
#define FROM_TRACE 0
#define LIN_WRITE 1
#define FROM_READ 2

//enum op_state {INVALID_, VALID_, SENT_, READY_, SEND_COMMITTS};
enum ts_compare{SMALLER, EQUAL, GREATER, ERROR};

struct ts_tuple {
  uint8_t m_id;
  uint8_t version[4];
};
// The format of an ack message
struct ack_message {
	uint8_t local_id[8]; // the first local id that is being acked
  uint8_t m_id;
  uint8_t opcode;
  uint16_t credits;
  uint16_t ack_num;
};


struct ack_message_ud_req {
	uint8_t grh[GRH_SIZE];
  struct ack_message ack;

 };


struct write {
  //uint8_t unused[2];
  //uint8_t w_num; // the first write holds the coalesce number for the entire message
  uint8_t m_id;
  uint8_t version[4];
  uint8_t key[TRUE_KEY_SIZE];	/* 8B */
  uint8_t opcode;
  uint8_t val_len;
  uint8_t value[VALUE_SIZE];
};

struct w_message {
  uint8_t l_id[8];
  uint8_t m_id;
  uint8_t w_num;
  struct write write[MAX_W_COALESCE];
};


struct w_message_ud_req {
  uint8_t unused[GRH_SIZE];
  struct w_message w_mes;
};

struct read {
  struct ts_tuple ts;
  uint8_t key[TRUE_KEY_SIZE];	/* 8B */
  uint8_t opcode;
};

struct r_message {
  uint8_t l_id[8];
  uint8_t coalesce_num;
  uint8_t m_id;
  struct read read[MAX_R_COALESCE];
};


struct r_message_ud_req {
  uint8_t unused[GRH_SIZE];
  struct r_message r_mes;
};

struct read_fifo {
  struct r_message *r_message;
  uint32_t push_ptr;
  uint32_t pull_ptr;
  uint32_t bcast_pull_ptr;
  uint32_t bcast_size; // number of reads not messages!
  uint32_t size;
  uint32_t backward_ptrs[R_FIFO_SIZE];
};

struct write_fifo {
  struct w_message *w_message;
  uint32_t push_ptr;
  uint32_t pull_ptr;
  uint32_t bcast_pull_ptr;
  uint32_t bcast_size; // number of prepares not messages!
  uint32_t size;
  uint32_t backward_ptrs[W_FIFO_SIZE];
};

// Sent when the timestamps are equal or smaller
struct r_rep_small {
  uint8_t opcode;
};


// Sent when you have a bigger ts_tuple
struct r_rep_big {
  uint8_t opcode;
  //uint8_t ts[TS_TUPLE_SIZE];
  struct ts_tuple ts;
  uint8_t value[VALUE_SIZE];

};

struct r_rep_message {
  uint8_t l_id[8];
  uint8_t coalesce_num;
  uint8_t m_id;
//  uint8_t credits;
  uint8_t opcode;
  struct r_rep_big r_rep[MAX_R_REP_COALESCE];
};


struct r_rep_message_ud_req {
  uint8_t unused[GRH_SIZE];
  struct r_rep_message r_rep_mes;
};

struct r_rep_fifo {
  struct r_rep_message *r_rep_message;
  uint8_t *rem_m_id;
  uint16_t *message_sizes;
  uint32_t push_ptr;
  //uint32_t inner_push_ptr; // this points to bytes, rather than "read replies"
  uint32_t pull_ptr;
  //uint32_t bcast_pull_ptr;
  uint32_t total_size; // number of r_reps not messages!
  uint32_t mes_size; // number of messages

};


// If a read
struct read_info {
  uint8_t rep_num;
  uint8_t times_seen_ts;
  bool seen_larger_ts;
	uint8_t opcode;
  struct ts_tuple ts_to_read;
  uint8_t key[TRUE_KEY_SIZE];
  uint8_t value[VALUE_SIZE];
};

struct pending_ops {
  struct write_fifo *w_fifo;
  struct read_fifo *r_fifo;
  struct write **ptrs_to_w_ops;
  struct read **ptrs_to_r_ops;
  struct r_message **ptrs_to_r_headers;
//  struct read_payload *r_payloads;
  struct read_info *read_info;
  struct r_rep_fifo *r_rep_fifo;
  uint64_t local_w_id;
  uint64_t local_r_id;
  uint32_t *r_session_id;
  uint32_t *w_session_id;

  uint8_t *w_state;
  uint8_t *r_state;
  uint32_t w_push_ptr;
  uint32_t r_push_ptr;
  uint32_t w_pull_ptr;
  uint32_t r_pull_ptr;
  uint32_t prep_pull_ptr; // Where to pull prepares from
  uint32_t w_size;
  uint32_t r_size;
  uint8_t *acks_seen;
  bool *session_has_pending_op;
  bool all_sessions_stalled;
};


struct recv_info {
	uint32_t push_ptr;
	uint32_t buf_slots;
	uint32_t slot_size;
	uint32_t posted_recvs;
	struct ibv_recv_wr *recv_wr;
	struct ibv_qp * recv_qp;
	struct ibv_sge* recv_sgl;
	void* buf;

};

struct fifo {
  void *fifo;
  uint32_t push_ptr;
  uint32_t pull_ptr;
  uint32_t size;

};


struct thread_stats { // 2 cache lines
	long long cache_hits_per_thread;

	long long reads_sent;
	long long acks_sent;
	long long r_reps_sent;
  long long writes_sent;


  long long reads_sent_mes_num;
  long long acks_sent_mes_num;
  long long r_reps_sent_mes_num;
  long long writes_sent_mes_num;


  long long received_reads;
	long long received_acks;
	long long received_r_reps;
  long long received_writes;

  long long received_r_reps_mes_num;
  long long received_acks_mes_num;
  long long received_reads_mes_num;
  long long received_writes_mes_num;


  uint64_t per_worker_acks_sent[MACHINE_NUM];
  uint64_t per_worker_acks_mes_sent[MACHINE_NUM];
  uint64_t per_worker_writes_received[MACHINE_NUM];
  uint64_t per_worker_acks_received[MACHINE_NUM];
  uint64_t per_worker_acks_mes_received[MACHINE_NUM];

  uint64_t per_worker_reads_received[MACHINE_NUM];
  uint64_t per_worker_r_reps_received[MACHINE_NUM];


	uint64_t read_to_write;
  uint64_t failed_rem_writes;
  uint64_t total_writes;


  uint64_t stalled_ack;
  uint64_t stalled_r_rep;

	//long long unused[3]; // padding to avoid false sharing
};

extern struct remote_qp remote_qp[MACHINE_NUM][WORKERS_PER_MACHINE][QP_NUM];
extern atomic_char qps_are_set_up;
extern struct thread_stats t_stats[WORKERS_PER_MACHINE];
struct mica_op;
extern atomic_uint_fast64_t global_w_id, committed_global_w_id;

struct thread_params {
	int id;
};

struct latency_counters{
	uint32_t* remote_reqs;
	uint32_t* local_reqs;
	uint32_t* hot_reads;
	uint32_t* hot_writes;
	long long total_measurements;
};


struct local_latency {
	int measured_local_region;
	uint8_t local_latency_start_polling;
	char* flag_to_poll;
};


extern struct latency_counters latency_count;

void *follower(void *arg);
void *leader(void *arg);
void *worker(void *arg);
void *print_stats(void*);

#endif
