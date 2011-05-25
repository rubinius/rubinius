---
layout: doc_pt_br
title: Profiler
previous: Debugger
previous_url: tools/debugger
next: Memory Analysis
next_url: tools/memory-analysis
---

Rubinius has an instrumenting profiler that provides precise timing for all
methods that are run. The profiler is implemented at the VM level and the data
is provided for Ruby code to process.


## VM Profiler

The cast of characters involved in creating and maintaining the profiler
include VM, SharedState, ProfilerCollection, and Profiler. The VM class is a
thread local data structure. Each VM instance gets a separate Profiler
instance. The SharedState instance has a ProfilerCollection instance that maps
VMs to Profilers and generates the composite results of all the Profilers.

The Profiler lives and dies in its own world. The profiler is passed a VM
instance when it is created because the profiler needs access to it while it
is gathering info. The STATE argument could be passed into all the profiler
methods, but it's simple enough to pass it in when the profiler is created.
The profiler never manipulates the VM instance. It is important to maintain
this separation.

The VM instance lazily creates its Profiler instance when needed. The VM
registers the profiler with the SharedState.

The SharedState maintains the ProfilerCollection instance and forwards calls
to register or remove profilers.

The ProfilerCollection instance requests that the VM instance mapped to a
profiler removes the profiler when the profiler will be deleted.


## Ruby Profiler

In Ruby land, the Rubinius::Profiler::Instrumenter instance exposes nothing
about the multi-threaded reality in the VM. The individual C++ Profiler
instances are not exposed to Ruby. In Ruby, a profiler instance is created.
That instance can simply start and stop the VM profiler. When the profiler is
stopped, info about the profiling is returned in a LookupTable. The Ruby
profiler code can display that info using the #show method.

Rubinius provides a compatible standard library profile.rb and profiler.rb.
Refer to MRI documentation on how to use those.

Looking at lib/profiler.rb, you can see the basic steps for using the profiler
from Ruby code.

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # start the profiler
    profiler.start

    # stop the profiler
    profiler.stop

    # get the profile data
    data = profiler.info

    # or print out the profiler info
    profiler.show  # takes on IO object, defaults to STDOUT

You can also use a convenience method to profile work in a block.

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # profile some work
    profiler.profile do
      # some work
    end

The #profile method starts the profiler, yields, stops the profiler and prints
the profile data by default. Pass 'false' to #profile to not print the data.
Either way, the profile data itself is returned by #profile.


How to Read the Flat Profiler Output
------------------------------------

The flat profiler output has the following columns:


### % time

The amount of time spent in this method as a percentage of the total time
spent in all methods.


### cumulative seconds

The total amount of time spent in this method and all its direct callees and
their callees all the way to every leaf method called along a path from this
method. Consider this method as the root of a call tree. The sum of all the
time spent in methods in this call tree is the cumulative seconds for this
method.


### self seconds

The total time spent in this method less the total time spent in all this
method's callees.


### calls

The total number of times this method was called.


### self ms/call

The self seconds as milliseconds divided by the total number of calls.


### total ms/call

The cumulative seconds as milliseconds divided by the total number of calls.


### Example of Flat Output

The following script is the basis of both profile examples below.

    class F
      def foo(a)
        1 + a
      end

      def bar(a, b)
        foo(a) ** b
      end

      def work(a, b, n, m)
        n.times { |i| i + bar(a, b) }
        m.times { |i| foo(i) }
      end
    end

    p = Rubinius::Profiler::Instrumenter.new :sort => :self_seconds
    p.profile {
      F.new.work 2, 3, 10, 5
    }


Running the script with 'bin/rbx script.rb' should give the following flat
output.


      %   cumulative   self                self     total
     time   seconds   seconds      calls  ms/call  ms/call  name
    ------------------------------------------------------------
      9.64     0.00      0.00         10     0.00     0.00  Fixnum#**
     23.33     0.00      0.00          1     0.01     0.08  #toplevel
     12.64     0.00      0.00         10     0.00     0.00  F#bar
     15.38     0.00      0.00         15     0.00     0.00  F#work {}
     17.74     0.00      0.00          2     0.00     0.03  Integer#times
     19.29     0.00      0.00          1     0.01     0.06  F#work
      1.12     0.00      0.00          1     0.00     0.00  Class#new
      0.68     0.00      0.00         15     0.00     0.00  F#foo
      0.13     0.00      0.00          1     0.00     0.00  Class#allocate
      0.06     0.00      0.00          1     0.00     0.00  Object#initialize

    10 methods called a total of 57 times


How to Read the Graph Output
----------------------------

The graph output is enabled with the configuration option:

    -Xprofiler.graph

Given the same script above, the graph output is shown below. Each "entry" in
the graph has three sections: 1) the method for the entry, called the
_primary_ line; 2) the callers of the primary method; and 3) the methods that
the primary method called. The fields have different meanings based on the
part of the entry.

For the primary line, the fields are as follows:


### index

An index assigned to each method in the graph to facilitate cross-referencing
the entries.


### % time

The amount of time spent in this method as a percentage of the total time
spent in all methods. This is the same as the flat output.


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

The total time spent in all the methods called by this method.


### called

The total number of times this method was called.


### name

The name of the method followed by the index number.


The lines above the primary line are methods that call the primary method. The
callers' fields have the following interpretation:


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

The time spent in the method's call to the primary method.


### called

The called field has two parts separated by a forward slash. The left is the
number of times this method called the primary method. The right is the total
number of calls this method made. In other words, the two numbers together
show a ratio of the calls to the primary method versus all calls made by the
caller.


### name

The name of the caller followed by its index number. If the index is [0], the
method does not appear in the graph.


The lines below the primary line are methods that the primary method called.
The fields for the called methods are as follows:


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

This is an estimate of the amount of time this method's callees spent when
this method was called by the primary method. The estimate is based on the
ration of the time this method spent when called by the primary method to the
total time spent in this method.


### called

The called field has two parts separated by a forward slash. The left is the
number of times this method was called by the primary method. The right is the
total number of times this method was called.


### name

The name of the called method followed by its index number [N]. If there is no
index present, there is no primary entry for the method in the graph. Use the
-Xprofiler.full_report option to print the entire graph if you need to view
the entry.


    index  % time     self  children         called       name
    ----------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
    [1]       9.6     0.00      0.00         10           Fixnum#** [1]
    -------------------------------------------------------
    [2]      23.2     0.00      0.00          1           #toplevel [2]
                      0.00      0.00          1/1              Class#new [7]
                      0.00      0.00          1/1              F#work [6]
    -------------------------------------------------------
                      0.00      0.00         10/15             F#work {} [4]
    [3]      12.7     0.00      0.00         10           F#bar [3]
                      0.00      0.00         10/15             F#foo [8]
                      0.00      0.00         10/10             Fixnum#** [1]
    -------------------------------------------------------
                      0.00      0.00         15/15             Integer#times [5]
    [4]      15.4     0.00      0.00         15           F#work {} [4]
                      0.00      0.00         10/10             F#bar [3]
                      0.00      0.00          5/15             F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          2/2              F#work [6]
    [5]      17.8     0.00      0.00          2           Integer#times [5]
                      0.00      0.00         15/15             F#work {} [4]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [6]      19.3     0.00      0.00          1           F#work [6]
                      0.00      0.00          2/2              Integer#times [5]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [7]       1.1     0.00      0.00          1           Class#new [7]
                      0.00      0.00          1/1              Object#initialize [10]
                      0.00      0.00          1/1              Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
                      0.00      0.00          5/15             F#work {} [4]
    [8]       0.7     0.00      0.00         15           F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [9]       0.1     0.00      0.00          1           Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [10]      0.1     0.00      0.00          1           Object#initialize [10]
    -------------------------------------------------------

    10 methods called a total of 57 times

