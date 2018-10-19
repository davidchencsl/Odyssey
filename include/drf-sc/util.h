#ifndef ARMONIA_UTILS_H
#define ARMONIA_UTILS_H

#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
//Multicast
#include <rdma/rdma_cma.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <netinet/in.h>
#include <netdb.h>
//



extern uint64_t seed;

/* ---------------------------------------------------------------------------
------------------------------STATS --------------------------------------
---------------------------------------------------------------------------*/
struct stats {
  double batch_size_per_thread[WORKERS_PER_MACHINE];
  double r_batch_size[WORKERS_PER_MACHINE];
  double r_rep_batch_size[WORKERS_PER_MACHINE];
  double ack_batch_size[WORKERS_PER_MACHINE];
  double write_batch_size[WORKERS_PER_MACHINE];
	double stalled_gid[WORKERS_PER_MACHINE];
  double stalled_ack[WORKERS_PER_MACHINE];
  double stalled_r_rep[WORKERS_PER_MACHINE];
	double failed_rem_write[WORKERS_PER_MACHINE];
  double quorum_reads_per_thread[WORKERS_PER_MACHINE];

	double cache_hits_per_thread[WORKERS_PER_MACHINE];

	double writes_sent[WORKERS_PER_MACHINE];
	double reads_sent[WORKERS_PER_MACHINE];
	double acks_sent[WORKERS_PER_MACHINE];
	double proposes_sent[WORKERS_PER_MACHINE];
	double rmws_completed[WORKERS_PER_MACHINE];
	double accepts_sent[WORKERS_PER_MACHINE];
	double commits_sent[WORKERS_PER_MACHINE];

	double r_reps_sent[WORKERS_PER_MACHINE];
	double received_writes[WORKERS_PER_MACHINE];
	double received_reads[WORKERS_PER_MACHINE];
	double received_acks[WORKERS_PER_MACHINE];
	double received_r_reps[WORKERS_PER_MACHINE];

	double reads_that_become_writes[WORKERS_PER_MACHINE];
};
void dump_stats_2_file(struct stats* st);
int spawn_stats_thread();
void print_latency_stats(void);


/* ---------------------------------------------------------------------------
------------------------------MULTICAST --------------------------------------
---------------------------------------------------------------------------*/
// This helps us set up the necessary rdma_cm_ids for the multicast groups
struct cm_qps
{
	int receive_q_depth;
	struct rdma_cm_id* cma_id;
	struct ibv_pd* pd;
	struct ibv_cq* cq;
	struct ibv_mr* mr;
	void *mem;
};

// This helps us set up the multicasts
struct mcast_info
{
	int	t_id;
	struct rdma_event_channel *channel;
	struct sockaddr_storage dst_in[MCAST_GROUPS_NUM];
	struct sockaddr *dst_addr[MCAST_GROUPS_NUM];
	struct sockaddr_storage src_in;
	struct sockaddr *src_addr;
	struct cm_qps cm_qp[MCAST_QPS];
	//Send-only stuff
	struct rdma_ud_param mcast_ud_param[MCAST_GROUPS_NUM];

};

// this contains all data we need to perform our mcasts
struct mcast_essentials {
	struct ibv_cq *recv_cq[MCAST_QP_NUM];
	struct ibv_qp *recv_qp[MCAST_QP_NUM];
	struct ibv_mr *recv_mr;
	struct ibv_ah *send_ah[MCAST_QP_NUM];
	uint32_t qpn[MCAST_QP_NUM];
	uint32_t qkey[MCAST_QP_NUM];
};

int get_addr(char*, struct sockaddr*);
void setup_multicast(struct mcast_info*, int*);
void resolve_addresses(struct mcast_info*);
void set_up_qp(struct cm_qps*, int*);
void multicast_testing(struct mcast_essentials*, int , struct hrd_ctrl_blk*);

/* ---------------------------------------------------------------------------
------------------------------INITIALIZATION --------------------------------------
---------------------------------------------------------------------------*/



// Worker calls this function to connect with all workers
void get_qps_from_all_other_machines(uint32_t g_id, struct hrd_ctrl_blk *cb);
// Used by all kinds of threads to publish their QPs
void publish_qps(uint32_t qp_num, uint32_t global_id, const char* qp_name, struct hrd_ctrl_blk *cb);

int parse_trace(char* path, struct trace_command **cmds, int g_id);


void trace_init(void **cmds, int g_id);
void init_multicast(struct mcast_info**, struct mcast_essentials**, int, struct hrd_ctrl_blk*, int);
// Connect with Workers and Clients
void setup_connections_and_spawn_stats_thread(uint32_t, struct hrd_ctrl_blk *);
/* ---------------------------------------------------------------------------
------------------------------ABD--------------------------------------
---------------------------------------------------------------------------*/

//Set up the depths of all QPs
void set_up_queue_depths(int**, int**);
// Initialize the rmw struct
void set_up_rmw_struct();
// Initialize the struct that holds all pending ops
void set_up_pending_ops(struct pending_ops**, uint32_t, uint32_t);
// Initialize the quorum info that contains the system configuration
void set_up_q_info(struct quorum_info **q_info);
// Set up the memory registrations in case inlining is disabled
// Set up the memory registrations required
void set_up_mr(struct ibv_mr **mr, void *buf, uint8_t enable_inlining, uint32_t buffer_size,
               struct hrd_ctrl_blk *cb);
// Set up all Broadcast WRs
void set_up_bcast_WRs(struct ibv_send_wr*, struct ibv_sge*, struct ibv_send_wr*,
                      struct ibv_sge*, struct ibv_recv_wr*, struct ibv_sge*,
                      struct ibv_recv_wr*, struct ibv_sge*, uint16_t,
                      struct hrd_ctrl_blk*, struct ibv_mr*, struct ibv_mr*);
// Set up the r_rep replies and acks send and recv wrs
void set_up_ack_n_r_rep_WRs(struct ibv_send_wr*, struct ibv_sge*, struct ibv_send_wr*,
                            struct ibv_sge*, struct ibv_recv_wr*, struct ibv_sge*,
                            struct ibv_recv_wr*, struct ibv_sge*,
                            struct hrd_ctrl_blk*, struct ibv_mr*,
                            struct ack_message*, uint16_t);


// construct a prep_message-- max_size must be in bytes
void init_fifo(struct fifo **fifo, uint32_t max_size, uint32_t);
// Set up the receive info
void init_recv_info(struct recv_info **recv, uint32_t push_ptr, uint32_t buf_slots,
                    uint32_t slot_size, uint32_t, struct ibv_recv_wr *recv_wr,
                    struct ibv_qp * recv_qp, struct ibv_sge* recv_sgl, void* buf);


// Post receives for the coherence traffic in the init phase
void pre_post_recvs(uint32_t*, struct ibv_qp *, uint32_t lkey, void*,
                    uint32_t, uint32_t, uint16_t, uint32_t);

// Set up the credits
//void set_up_credits(uint16_t credits[][MACHINE_NUM]);
void set_up_credits(uint16_t credits[][MACHINE_NUM]);
// Manufactures a trace without a file
void manufacture_trace(struct trace_command_uni **cmds, int t_id);



/* ---------------------------------------------------------------------------
------------------------------UTILITY --------------------------------------
---------------------------------------------------------------------------*/

// pin threads starting from core 0
int pin_thread(int t_id);
// pin a thread avoid collisions with pin_thread()
int pin_threads_avoiding_collisions(int c_id);

void print_latency_stats(void);



#endif /* ARMONIA_UTILS_H */
