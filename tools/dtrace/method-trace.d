/*
 * method trace
 *
 * This script prints a trace of what methods are entered as your ruby program is executing.
 * Enhancements to indent the flow, etc, are pending :)
 *
 * Example invocation:
 *
 * $> sudo dtrace -s method-trace.d -c 'rbx ~/hello.rb'
 */

#pragma D option quiet
#pragma D option dynvarsize=250m
#pragma D option bufsize=64m

rubinius$target:::function-entry
{
	printf("%s:%s\n", copyinstr(arg0), copyinstr(arg1));
}

