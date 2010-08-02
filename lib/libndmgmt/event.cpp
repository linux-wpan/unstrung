/*
 * Copyright (C) 2009 Michael Richardson <mcr@sandelman.ca>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 */

/*
 * this file is just for setup and maintenance of interface,
 * it does not really do any heavy lifting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include <errno.h>
#include <pathnames.h>		/* for PATH_PROC_NET_IF_INET6 */
#include <poll.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/if.h>           /* for IFNAMSIZ */
#include <time.h>
#include "rpl.h"

#ifndef IPV6_ADDR_LINKLOCAL
#define IPV6_ADDR_LINKLOCAL   0x0020U
#endif
}

#include "iface.h"
#include "event.h"

void rpl_event::doit(void)
{
}

const char *rpl_event::event_name()
{
    switch(event_type) {
    case rpl_send_dio:
        return "send_dio";
    }

    return "<unknown>";
}

void rpl_event::printevent(FILE *out)
{
    char b1[256];
    struct tm tm1;

    gmtime_r(&alarm_time.tv_sec, &tm1);

    strftime(b1, sizeof(b1), "%Y-%B-%d %r", &tm1);
    fprintf(out, "event(%s) at (%s)<%d:%d>, type: %s",
            mReason,  b1, alarm_time.tv_sec, alarm_time.tv_usec,
            event_name());
}

struct timeval rpl_event::occurs_in(struct timeval &now) {
    int usec_interval = alarm_time.tv_usec - now.tv_usec;
    int borrow = 0;
    struct timeval diff;

    if(usec_interval < 0) {
        usec_interval += 1000000;
        borrow = 1;
    } 

    diff.tv_usec = usec_interval;
    diff.tv_sec = alarm_time.tv_sec - now.tv_sec - borrow;
    return diff;
}

struct timeval rpl_event::occurs_in() {
    struct timeval now;
    
    gettimeofday(&now, NULL);
    return occurs_in(now);
}


void printevents(FILE *out, event_map em) {
    int i = 1;
    event_map_iterator one = em.begin();
    while(one != em.end()) {
        rpl_event &n = one->second;
        fprintf(out, "%d: ", i);
        n.printevent(out);
        fprintf(out, "\n");
        i++; one++;
    }
}


/*
 * Local Variables:
 * c-basic-offset:4
 * c-style: whitesmith
 * End:
 */