---
layout: doc_pt_br
title: Memory Analysis
previous: Profiler
previous_url: tools/profiler
next: How-To
next_url: how-to
review: true
---

Rubinius provides an interface for dumping the current heap to a file for
offline analysis. Several companion projects analyze the heap dump file
as an aid for discovering memory leaks, large collections and code issues
that may lead to potential memory pressures at runtime.

## A Sample Program

The following sample code (devoid of all error checks) will form the foundation
for a tour through tracking a memory leak in Ruby code as well as a leak in code 
using the FFI subsystem.

The code example is a bit contrived, but it serves to illustrate multiple real
world problems.

    require 'rubygems'
    require 'ffi-rzmq'
    
    if ARGV.length < 3
      puts "usage: ruby leak.rb <connect-to> <message-size> <roundtrip-count>"
      exit
    end
    
    link = ARGV[0]
    message_size = ARGV[1].to_i
    roundtrip_count = ARGV[2].to_i
    
    ctx = ZMQ::Context.new
    request_socket = ctx.socket ZMQ::REQ
    reply_socket = ctx.socket ZMQ::REP
    
    request_socket.connect link
    reply_socket.bind link
    
    poller = ZMQ::Poller.new
    poller.register_readable request_socket
    poller.register_readable reply_socket
    
    start_time = Time.now
    
    message = ZMQ::Message.new("a" * message_size)
    request_socket.send message, ZMQ::NOBLOCK
    i = roundtrip_count
    messages = []
    
    until i.zero?
      i -= 1
    
      poller.poll_nonblock
    
      poller.readables.each do |socket|
        message = ZMQ::Message.new
        socket.recv message, ZMQ::NOBLOCK
        messages << message
        socket.send ZMQ::Message.new(message.copy_out_string), ZMQ::NOBLOCK
      end
    end
    
    elapsed_usecs = (Time.now.to_f - start_time.to_f) * 1_000_000
    latency = elapsed_usecs / roundtrip_count / 2
    
    puts "mean latency: %.3f [us]" % latency
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

Wow, this program leaks like a sieve. Let's figure out why.

## Saving A Heap Dump

Rubinius provides access to the VM via an agent interface. The agent opens a network
socket and responds to commands issued by the console program. The agent must be 
started with the program.

    rbx -Xagent.start <script name>

For this example, run the sample program with the agent enabled.

    rbx -Xagent.start leak.rb

Connect to the agent using the rbx console. This program opens an interactive session
with the agent running inside the VM. Commands are issued to the agent. In this case 
we are saving a heap dump for offline analysis.

Upon startup, the agent writes a file to $TMPDIR/rubinius-agent.\<pid\> containing a
few important details for the rbx console. When exiting, the agent automatically
cleans up this file and removes it. Under some crash conditions this file may not
get removed so a manual removal becomes necessary.

    $ rbx console
    VM: rbx -Xagent.start leak.rb tcp://127.0.0.1:5549 1024 100000000
    Connecting to VM on port 60544
    Connected to localhost:60544, host type: x86_64-apple-darwin10.5.0
    console> set system.memory.dump heap.dump
    console> exit

The command is `set system.memory.dump <filename>`. The heap dump file is written to
the current working directory for the program running the agent.

## Analyzing A Heap Dump

The heap dump file is written using a well-documented format. So far there are two
tools that know how to read and interpret the format. These projects are separate
from the Rubinius project.

Find the heap_dump tool at [its project home page](https://github.com/evanphx/heap_dump).

This tool reads the heap dump file and outputs some useful information in 3 columns
corresponding to the number of objects visible in the heap, the object's class, and
the total number of bytes consumed by all instances of this object.

Running the tool against a heap dump captured from our `leak.rb` program, it gives us
a small hint as to where the leak resides. The output is edited for length.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump 
        169350   Rubinius::CompactLookupTable 21676800
        168983             FFI::MemoryPointer 6759320
        168978                   ZMQ::Message 8110944
        168978                    LibZMQ::Msg 6759120
         27901                Rubinius::Tuple 6361528
         15615                         String 1124280
         13527            Rubinius::ByteArray 882560
          3010                          Array 168560
           825                    Hash::Entry 46200
           787       Rubinius::AccessVariable 62960
            87                           Time 4872
            41                           Hash 3280
            12                   FFI::Pointer 480
             2                    ZMQ::Socket 96

Nothing listed looks too outrageous in this example. However, a few things are notable.

1. The largest footprint is consumed by `Rubinius::CompactLookupTable` which is a 
class that the example code never directly instantiates and weighs in at about 20MB. 
So, some internal Rubinius structures are reported by the heap dump. It is 
interesting but doesn't help pinpoint our particular leak.

2. The `ZMQ::Message` class listed on line 3 is the first class shown that the example
code directly instantiates. There are nearly 170k instances, so this is likely our
leak.

Sometimes a single snapshot is insufficient to pinpoint a leak. In that situation
one should take several snapshots of the heap at different times and let the
heap dump tool perform a *diff* analysis. The *diff* shows what has changed between
the heap in the *before* and *after*.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap.dump heap2.dump
    203110   Rubinius::CompactLookupTable 25998080
    203110                   ZMQ::Message 9749280
    203110                    LibZMQ::Msg 8124400
    203110             FFI::MemoryPointer 8124400

The diff clearly shows us the source of the memory expansion. The code has 200k more
instances of `ZMQ::Message` between the first and second heap dumps so that is where
all of the memory growth is coming from.

Examining the code shows two lines as the likely culprit.

    messages << message
    ...
    puts "received #{messages.size} messages in #{elapsed_usecs / 1_000_000} seconds"

It certainly is not necessary to store every message just to get a count at the
end. Revising the code to use a simple counter variable in its place should solve
the memory leak.

## Advanced Tools - OSX Only

After modifying the Ruby code to use a simple counter and let the garbage collector
handle all of the `ZMQ::Message` instances, the program is still leaking like mad.
Taking two snapshots and analyzing them doesn't give any clue as to the source
though.

    $ rbx -I /path/to/heap_dump/lib /path/to/heap_dump/bin/histo.rb heap3.dump heap4.dump
      -4                          Array -224
     -90                 Digest::SHA256 -4320
     -90          Rubinius::MethodTable -4320
     -90                   Digest::SHA2 -3600
     -90          Rubinius::LookupTable -4320
     -90                          Class -10080
    -184                Rubinius::Tuple -29192

This diff shows that a few structures actually shrank between snapshots. Apparently
the leak is no longer in the Ruby code because the VM cannot tell us what is
consuming the leaking memory.

Luckily there is a great tool available on Mac OS X called `leaks` that can help us
pinpoint the problem. Additionally, the man page for `malloc` contains information
about setting an environment variable that provides additional detail to the
`leaks` program such as the stack trace at the site of each leak.

    $ MallocStackLogging=1 rbx leak.rb tcp://127.0.0.1:5549 1024 10000000 &
    $ leaks 36700 > leak.out
    $ vi leak.out
    leaks Report Version:  2.0
    Process:         rbx [36700]
    Path:            /Volumes/calvin/Users/cremes/.rvm/rubies/rbx-head/bin/rbx
    Load Address:    0x100000000
    Identifier:      rbx
    Version:         ??? (???)
    Code Type:       X86-64 (Native)
    Parent Process:  bash [997]
    
    Date/Time:       2010-12-22 11:34:35.225 -0600
    OS Version:      Mac OS X 10.6.5 (10H574)
    Report Version:  6
    
    Process 36700: 274490 nodes malloced for 294357 KB
    Process 36700: 171502 leaks for 263427072 total leaked bytes.
    Leak: 0x101bb2400  size=1536  zone: DefaultMallocZone_0x100dea000
            0x01bb2428 0x00000001 0x00000400 0x00000000     ($..............
            0x00000000 0x00000000 0x00000000 0x00000000     ................
            0x00000000 0x00000000 0x61616161 0x61616161     ........aaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            0x61616161 0x61616161 0x61616161 0x61616161     aaaaaaaaaaaaaaaa
            ...
            Call stack: [thread 0x102f81000]: | thread_start | _pthread_start | 
            thread_routine | zmq::kqueue_t::loop() | zmq::zmq_engine_t::in_event() | 
            zmq::decoder_t::eight_byte_size_ready() | zmq_msg_init_size | malloc | 
            malloc_zone_malloc

The output shows that at the time of the snapshot we had nearly 172k leaked objects.
The call stack output shows that the leak occurs during the call to `zmq_msg_init_size`
which doesn't mean anything unless we dig into the implementation of `ZMQ::Message``.
This is where knowledge of the underlying system is critical; without knowing where
this particular call is made, it would be much more difficult to track down the
problem.

As it turns out, the `ZMQ::Message` class allocates some memory via `malloc` that is not
tracked by the Rubinius GC. It needs to be manually deallocated.

Changing the code to call `ZMQ::Message#close` resolves the last leak.
