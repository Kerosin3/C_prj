#include "event_loop.h"
#include "connection_handlers.h"
#include "ipcmessages.h"
#include "misc.h"
#include "setup.h"


void event_loop(int sockfd, struct io_uring *ring)
{

    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    int current_client_fd = -1;
    int sndsize = 65536;
    int err;
    if ((err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_SNDBUF, (char *)&sndsize, (int)sizeof(sndsize))))
        strerror(err);
    if ((err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_RCVBUF, (char *)&sndsize, (int)sizeof(sndsize))))
        strerror(err);
    unsigned long long ii = 0;
    add_accept_request(ring, sockfd, &client_addr, &client_addr_len);
    for(;;){

        struct io_uring_cqe *cqe;
	if (UNLIKELY(io_uring_wait_cqe(ring, &cqe)))
            die("error accepting a connection");
        //printf("current fd is %d, cycle serial:%llu\n", cqe->res, ii);
        switch (request_data_event_type(cqe->user_data)) // get type EVENT
	{
		case FLAG_ACCEPT:
			printf("Flag accept!\n");
            		add_accept_request(ring, sockfd, &client_addr,
                               &client_addr_len); // add requst one more time  and set socket id

			printf("current client socket: %d \n",cqe->res);
             		current_client_fd = request_data_client_fd(cqe->res); // get current fd
	            	//buffer_lengths[cqe->res] = 0;         // set current read buffer 0
        	        set_flags(cqe->res);                  // set flags for the socket OK??
			request_ASK_NEED_MSG(ring,cqe->res); // send request ask MSG
			break;
        	case READ_RESPONSE: // wait response
			printf("sended request msg %d\n",cqe->res);
	            if (LIKELY(cqe->res)) {// received some bytes
			printf("WAIT WAITING RESPONSE FROM %d \n",request_data_client_fd(cqe->user_data));
			READ_STATUS_RESPONSE(ring,request_data_client_fd(cqe->user_data)); // WILL BE WAIT TILL RECV WORKS
			//printf("received %d\n",cqe->res);
			//DumpHex(get_client_buffer(cqe->user_data),cqe->res);
			//printf("status->\n");
		    }
			break;
	            //if (LIKELY(cqe->res)) {// non-empty request?  set fd test not zero read
		//		handle_response_IFNEED_MESSAGE(ring, request_data_client_fd(cqe->user_data));
		//	}
		case WAIT_ACKNOWLEDGEMENT:
			printf("WAITING ACK!\n");
//			request_ASK_NEED_MSG(ring,cqe->res); // send request ask MSG

			break;
		case TEST_RESPONSE:
	            if (LIKELY(cqe->res)) {// non-empty request?  set fd test not zero read
			printf("TEST FOR RESPONSE, readed %d bytes \n",cqe->res);
			DumpHex(get_client_buffer(cqe->user_data),cqe->res);
			IpcMessage__Status STATUS;
			switch ( STATUS = read_ONLY_STATUS_response(get_client_buffer(cqe->user_data),cqe->res)) {
				case (IPC_MESSAGE__STATUS__NEED_MORE):
					printf("NEED MORE MESSAGES!\n");
					handle_response_NEED_MORE_MSG(ring, cqe->user_data);
					printf("SEND MESSAGE!\n");
					break;
				case (IPC_MESSAGE__STATUS__ACKN_OK):
					printf("AKNOWLEDGED!\n");
					break;
			}
		    }
	                break;
		printf("out cycle\n");
	}
	/* when??
        shutdown(current_client_fd, SHUT_RDWR);
        int closeret = close(current_client_fd);
        if (closeret < 0)
               printf("error while closing socket %d, %s\n", current_client_fd, strerror(errno));
 
 	*/
        io_uring_cqe_seen(ring, cqe);
    }
}



