#include "dhandlers_p2p.h"

int request_data_client_fd(uint64_t request_data)
{
    // UNIT_MAX = 0x00000000FFFFFFFF
    return request_data & UINT_MAX;
}

STATE_P2P request_data_event_type(uint64_t request_data)
{
    uint64_t mask = ULLONG_MAX - UINT_MAX;
    // ULLONG_MAX - UINT_MAX = 0xFFFFFFFF00000000
    uint64_t ret = (request_data & (mask)) >> 32;
    return (STATE_P2P)ret;
}

char *get_client_buffer(int client_fd)
{
    return &buffers[client_fd * BUFFER_SIZE];
}
//  ---flag---clientfd

u_int64_t make_request_data(int client_fd, STATE_P2P flag)
{
    return (uint64_t)(flag) << 32 | client_fd;
}

//-------------------------------------------------------//
void add_accept_request(struct io_uring *ring, int serverfd, struct sockaddr_in *a_client_adrd,
                        socklen_t *client_addr_len)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring); // pop
    if (!sqe)
    {
        printf("error adding to que\n");
    }
    io_uring_prep_accept(sqe, serverfd, (struct sockaddr *)a_client_adrd, client_addr_len, 0);
    io_uring_sqe_set_data64(sqe, make_request_data(0, FLAG_ACCEPT));
    if (io_uring_submit(ring) < 0)
        printf("error submitting\n");
    if (p2p_logging_enabled) zlog_info(p2p_log, "p2p set ACCEPT state");
}


//--------------------------------------------//
//

void set_flags(int socket)
{
    size_t sndsize = 65536;
    int err;

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    if ((err = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_SNDBUF, (char *)&sndsize, (int)sizeof(sndsize))))
        strerror(err);
    if ((err = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_RCVBUF, (char *)&sndsize, (int)sizeof(sndsize))))
        strerror(err);
    if ((err = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) < 0)
        strerror(err);
    if ((err = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) < 0)
        strerror(err);
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
       strerror("setsockopt(SO_REUSEADDR) failed");
}
