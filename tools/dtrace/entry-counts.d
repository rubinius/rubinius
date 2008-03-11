/*
 * entry/exit counts
 *
 * This script manages a counter for every method invocation. Upon function-entry the probe increments
 * the counter, upon function-return the counter decrements the counter. At the end of the run
 * dtrace should print all methods run, with a count of 0 for all of them - indicating that the probes
 * have fired correctly.
 *
 * Example invocation:
 *
 * $> sudo dtrace -s entry-counts.d -c 'rbx ~/hello.rb'
 */

#pragma D option dynvarsize=250m
#pragma D option bufsize=64m
#pragma D option quiet

rubinius$target:::function-entry { @[copyinstr(arg0), copyinstr(arg1)] = sum(1); } 
rubinius$target:::function-return { @[copyinstr(arg0), copyinstr(arg1)] = sum(-1); } 

