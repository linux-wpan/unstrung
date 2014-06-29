#include <stdio.h>
#include <stdlib.h>
#include "iface.h"

#include "fakeiface.h"

int main(int argc, char *argv[])
{
        pcap_network_interface *iface = NULL;
        struct in6_addr iface_src2;

        rpl_debug *deb = new rpl_debug(true, stdout);
        deb->want_time_log = false;

        dag_network *dag = new dag_network("ripple", deb);
        dag->set_active();
        dag->set_interval(5000);

        /* now finish setting things up with netlink */
        pcap_network_interface::scan_devices(deb, false);

        inet_pton(AF_INET6, "fe80::1000:ff:fe64:6602", &iface_src2);

        const char *pcapin1 = "../INPUTS/dio-A-ripple.pcap";
        iface = pcap_network_interface::setup_infile_outfile("wlan0", pcapin1, "../OUTPUTS/24-node-E-out.pcap", deb);

	struct timeval n;
	n.tv_sec = 1024*1024*1024;
	n.tv_usec = 1024;
	iface->set_fake_time(n);

        iface->set_debug(deb);
        iface->set_if_index(1);
        iface->set_if_addr(iface_src2);
        iface->watching = true;

        dag->set_debug(deb);

        printf("Processing input file %s on if=[%u]: %s state: %s %s\n",
               pcapin1,
               iface->get_if_index(), iface->get_if_name(),
               iface->is_active() ? "active" : "inactive",
               iface->faked() ? " faked" : "");
        iface->process_pcap();

        /* now drain off any created events */
        network_interface::terminating();
        while(network_interface::force_next_event());

        const char *pcapin2 = "../INPUTS/daoack-A-ripple.pcap";
        iface = pcap_network_interface::setup_infile_outfile("wlan0", pcapin2, NULL, deb);
        printf("Processing input file %s on if=[%u]: %s state: %s %s\n",
               pcapin2,
               iface->get_if_index(), iface->get_if_name(),
               iface->is_active() ? "active" : "inactive",
               iface->faked() ? " faked" : "");
        iface->process_pcap();

        /* again, drain off any events */
        network_interface::terminating();
        while(network_interface::force_next_event());

	exit(0);
}


