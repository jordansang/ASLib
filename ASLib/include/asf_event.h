#ifndef ASF_EVENT_H
#define ASF_EVENT_H

struct asf_event_item
{
    s_int32_t eid;
};
struct asf_event
{
    struct asl_sem esem;
    struct asl_thread_mutex listMutex;
    struct asd_list *eventList;
    struct asl_thread_mutex fifoMutex;
    struct asd_fifo *eventFifo;
};

struct asf_event* asf_event_create();
result_t asf_event_destroy(struct asf_event *ae);
s_int32_t asf_event_get_eid(struct asf_event *ae);
result_t asf_event_post(struct asf_event *ae, s_int32_t eid);
s_int32_t asf_event_pend(struct asf_event *ae);
result_t asf_event_release_eid(struct asf_event *ae, s_int32_t eid);


#endif /* ASF_EVENT_H */
