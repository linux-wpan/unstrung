#!/bin/sh

set -e
mkdir -p $HOME/stuff
BUILDTOP=$(cd $HOME/stuff; pwd)

if [ ! -x $HOME/stuff/host/tcpdump-4.7.4/tcpdump ]
then
    (cd ${BUILDTOP} && curl http://www.ca.tcpdump.org/release/libpcap-1.7.4.tar.gz | tar xzf - )
    (cd ${BUILDTOP} && curl http://www.ca.tcpdump.org/release/tcpdump-4.7.4.tar.gz | tar xzf - )
    (cd ${BUILDTOP} && mkdir -p host/libpcap-1.7.4 && cd host/libpcap-1.7.4 && ../../libpcap-1.7.4/configure --prefix=${BUILDTOP} && make && make install)
    (cd ${BUILDTOP} && mkdir -p host/tcpdump-4.7.4 && cd host/tcpdump-4.7.4 && ../../tcpdump-4.7.4/configure --prefix=${BUILDTOP} && make && make install)
    (cd ${BUILDTOP} && mkdir -p x86/libpcap-1.7.4 && cd x86/libpcap-1.7.4 && CFLAGS=-m32 ../../libpcap-1.7.4/configure --target=i686-pc-linux-gnu && make LDFLAGS=-m32 CFLAGS=-m32)
    (cd ${BUILDTOP} && ln -s x86/libpcap-1.7.4 libpcap && mkdir -p x86/tcpdump-4.7.4 && cd x86/tcpdump-4.7.4 && CFLAGS=-m32 ../../tcpdump-4.7.4/configure --target=i686-pc-linux-gnu && make LDFLAGS=-m32 CFLAGS=-m32)
fi

echo LIBPCAP=${BUILDTOP}/x86/libpcap-1.7.4/libpcap.a >Makefile.local
echo LIBPCAP_HOST=${BUILDTOP}/host/libpcap-1.7.4/libpcap.a >>Makefile.local
echo LIBPCAPINC=-I${BUILDTOP}/include                >>Makefile.local
echo ARCH=i386  			             >>Makefile.local
echo TCPDUMP=${BUILDTOP}/host/tcpdump-4.7.4/tcpdump  >>Makefile.local
echo NETDISSECTLIB=${BUILDTOP}/host/tcpdump-4.7.4/libnetdissect.a >>Makefile.local
echo NETDISSECTH=-DNETDISSECT -I${BUILDTOP}/include -I${BUILDTOP}/host/tcpdump-4.7.4/ -I${BUILDTOP}/tcpdump-4.7.4 >>Makefile.local
echo export ARCH LIBPCAP LIBPCAP_HOST LIBPCAPINC TCPDUMP NETDISSECTLIB NETDISSECTH >>Makefile.local
