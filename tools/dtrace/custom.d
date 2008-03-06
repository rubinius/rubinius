/*
 * custom rubinius probe.
 *
 * This script prints the parameters passed to an arbitrary custom probe
 * within rubinius. arg0 and arg1 are the parameters passed to Dtrace.fire().
 * custom probes are useful to enable/disable other probes within scripts
 * or just to pass arbitrary data through to dtrace.
 *
 * Example invocation:
 *
 * $> sudo dtrace -s entry-counts.d -c 'rbx custom.rb'
 *
 * Example script:
 *
 * $> cat custom.rb
 * Dtrace.fire 'hello', 'world'
 */

#pragma D option quiet

rubinius$target:::ruby-probe
{
	printf("%s:%s\n", copyinstr(arg0), copyinstr(arg1));
}

