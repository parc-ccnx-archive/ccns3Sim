
# CCNx Module for NS3

This code represents the CCNx 1.0 Protocol (see IRTF ICNRG documents
irtf-icnrg-ccnx-semantics-01.txt and irtf-icnrg-ccnx-messages-01.txt).

# Installation

## Dependencies and configuration

    brew install valgrind
    brew install libxml2
    
    # To build doxygen (yes mercurial is required for ./waf docs)
    brew install mercurial
    brew install doxygen
    brew install graphviz
    brew install dia
    
    # In the ns-3.24 directory
    ./waf configure --enable-examples --enable-tests --disable-python --disable-nsclick --disable-gtk

## Building

    # In the ns-3.24 directory
    ./waf 
    
    # To build the documentation.  The CCNx docs will be under Modules/CCNx 1.0 Protocol.
    ./waf doxygen
    open doc/html/index.html
    
## Running

### Examples

Run the 1 node example:

    ./waf --run ccnx-simple

Run the 1 node example in the debugger:

    ./waf --run ccnx-simple --command-template="gdb %s"
    ./waf --run ccnx-simple --command-template="lldb -- %s

### Unit Tests

 To see what test suites are available:

     ./test.py --list | grep CCNx
     unit         TestSuiteCCNxBuffer
     unit         TestSuiteCCNxName
     unit         TestSuiteCCNxNameSegment
     unit         TestSuiteCCNxTime

To run a test suite (for example CCNxNameSegment):

    ./test.py -s TestSuiteCCNxNameSegment -v
    or
    ./waf --run test-runner --command-template="%s --suite=TestSuiteCCNxNameSegment --verbose"

To debug a test:

    ./waf --run test-runner --command-template="gdb %s --args --suite=TestSuiteCCNxNameSegment --verbose"

If that does not work, then do this:

    ./waf --run test-runner --command-template="gdb %s"
    (gdb) set args --suite=TestSuiteCCNxNameSegment --verbose

If using lldb:

    ./waf --run test-runner --command-template="lldb -- %s --suite=TestSuiteCCNxCodecFixedHeader --verbose"

To run a test and see the results of valgrind:

    ./test.py -s TestSuiteCCNxCodecFixedHeader -g -v

 NOTE: I've not gotten these to fully pass, but not due to memory leaks in the CCNx code.  It seems to be issues
 the stuff created in NS-related statics.
 
# Notes on the code

Code uses the NS3 Smart Pointer and unit test mechanisms.

- IDE is CLion.  Does not build in IDE, must still use command-line waf.  The checked-in CMakeLists.txt is close,
  but not there yet.

- KeyId, KeyIdRest, ObjectHashRest use uint64_t instead of a 32-byte buffer.  The codec knows to pad out to the right
  length.  The uint64_t should be at least typedef'd to their own type if not a shell C++ class.

- The ns3 file nameing scheme is very different from what I used (which is what we use in Distillery).  Files should
  be renamed to something like ccnx-l3-protocol or ccnx-interest.

- The ns3 convention is to use camel case.  Methods are initial capitalized, unlike Distillery.  There are some
  of the early ccnx classes in ns3 that used lower case.  They should be converted to the ns3 convention.

- ns3 uses a GNU code style convention. It has its own uncrustify script (from inside ns-3.24 directory):

   find src/ccnx \( -name '*.h' -or -name '*.cpp' \) -exec utils/check-style.py  -l 3 -i "" -f {} \;

- There's a mixture of #include "ns3/x" and #include <ns3/x>.  Should pick one.

- I've not looked at the ns3 headerdoc convention.  Need to document (or reference) what that is and use it.

- ns3 uses ".cc" and I used ".cpp".  Should change to ".cc".

## General TODO Items

- Documentation
   - Create the \defgroup CCNx then start creating subgroups and put them in the CCNx group

## General NS3 Protocol Architecture

TODO: Add picture of te Node and how Portals attach to it

The CCNx protocol attached to a node is very similar in architecture to how ns3 did the ipv4 stack.
A CCNxL3Protocol is aggregated to a node and serves as the basis for the CCNx protocol.  However, how CCNx
handles routing in NS3 differs some from IPv4. Forwarding is potentially a more complex task with more
options about how to do it than in IP, and one may want to change the forwarding independent of changing
the routing protocol.  Therefore, we use this class architecture:


	  +-----------------------+   +-----------------------+  +-------------------+
	  | CCNxStandardForwarder |  -| (routing protocol [1])|  | CCNxMessagePortal |
	  +-----------------------+ / +-----------------------+  +---------------------------+
	  |  CCNxForwarder        |-  | CCNxRoutingProtocol   |  | CCNxPortal (abstract)     |
	  |   (abstract)          |   |    (abstract)         |  | CCNxL4Protocol (abstract) |
	  +-----------------------+   +-----------------------+  +---------------------------+
	  RouteInput()/RouteOutput()  |     CCNxPortal        |    Receive() / SendCallback()
	  +---------------------------+-----------------------+-----------------+
	  |                          CCNxL3Protocol                             |
	  +---------------------------------------------------------------------+
	    Receive() / Send()
	   +---------------+
	   |  NetDevice    |
	   +---------------+


[1] Included: CCNxStaticRouting, NfpRoutingProtocol

Order of construction:

- NOTE: You don't need to do this, it's part of the `CCNxStackHelper` and `CCNxRoutingHelper`.
- Aggregate `CCNxL3Protocol` on the node
- Aggregate `CCNxMessagePortalFactory` on the node
- Instantiate L2 devices
- Attach L2 devices to `CCNxL3Protocol`
- Aggregate a forwarder (e.g. `CCNxStandardForwarder`) on the node.
- call `CCNxL3Protocol::SetForwarder(Ptr<CCNxForwarder>)`
- Aggregate a routing protocol (e.g. `CCNxStaticRouting`) on the node.
    - it will use a standard CCNxPortal interface to send/receive packets.
    - it will use public methods on `CCNxL3Protocol` to learn about L2 devices.
    - it may use `CCNxPortal::Send(Ptr<CCNxPacket>, uint32 connectionId)` to send to a specific peer.
- CCNxMessagePortal and CCNxMessagePortalFactory use the socket-like architecture to send/receive.
    - `RegisterPrefix(Ptr<CCNxName>)` creates a local FIB entry pointing to a L4 protocol.
    - `RegisterAnchor(Ptr<CCNxName>)` does RegisterPrefix and notifies the routing protocol to anchor the name (advertise it).

The RoutingProtocol can talk with the CCNxForwarder by doing a `LookupByName("ns3::ccnx::CCNxForwarder")` and get
a `Ptr<CCNxForwarder>` reference to whatever forwarder is instantiated on the node.  This will let it manipulate the FIB.

If a user wants to implement a non-standard FIB and routing protocol combination, use the same architecture.  But
in this case, the routing protocol could do a `LookupByName("ns3::ccnx::FancyForwarder")` and have access to the full range of
functions available in the forwarder beyond the standard FIB methods provided in CCNxForwarder.  FancyForwarder
would still see all the calls to `RouteInput()` and `RouteOutput()` so it would have the same complete flexibility to
do whatever it wants.

## CCNx Applications and Portals

A CCNx application should use one of the portal factories from
portals/ to create a `CCNxPortal` to send/receive CCNxPackets.

The CCNxPortal implementation here is similar to the NS3 Socket.
There's a `CCNxPortal` and `CCNxPortalFactory`.  The `CCNxPortalFactory`
is used by a transport protocol to create a `CCNxPortal` bound to a
specific transport.

For example:

    Ptr<CCNxPortal>
    foo(Ptr<Node> node)
    {
     TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");
     Ptr<CCNxPortal> portal = CCNxPortal::CreatePortal (node, tid);
     return portal;
    }

Portals:

CCNxMessagePortal -
A message-by-message Portal.

CCNxFlicPortal -
A transport based on FLIC manifests.

CCNxChunkPortal -
Retrieves a chunked object when the application sends a first interest.
It will then generate all the chunked names and retrieve them via
a TCP-like flow controller.  It will deliver the chunks in-order.


# Protocol Notes

* Ethertype: 0x0801
* PPP Protocol Number: 0x0025

# LICENSE
<pre>
Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

################################################################################
#
# PATENT NOTICE
#
# This software is distributed under the BSD 2-clause License (see LICENSE
# file).  This BSD License does not make any patent claims and as such, does
# not act as a patent grant.  The purpose of this section is for each contributor
# to define their intentions with respect to intellectual property.
#
# Each contributor to this source code is encouraged to state their patent
# claims and licensing mechanisms for any contributions made. At the end of
# this section contributors may each make their own statements.  Contributor's
# claims and grants only apply to the pieces (source code, programs, text,
# media, etc) that they have contributed directly to this software.
#
# There is no guarantee that this section is complete, up to date or accurate. It
# is up to the contributors to maintain their section in this file up to date
# and up to the user of the software to verify any claims herein.
#
# Do not remove this header notification.  The contents of this section must be
# present in all distributions of the software.  You may only modify your own
# intellectual property statements.  Please provide contact information.

- Palo Alto Research Center, Inc
This software distribution does not grant any rights to patents owned by Palo
Alto Research Center, Inc (PARC). Rights to these patents are available via
various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
intellectual property used by its contributions to this software. You may
contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
</pre>