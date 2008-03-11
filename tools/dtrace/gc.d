/*
 * garbage collection.
 *
 * This script prints statistics about the Rubinius garbage collectors as they're running
 * to clean up memory.
 *
 * Example invocation:
 *
 * $> sudo dtrace -s gc.d -c 'rbx your-long-running-application.rb'
 *
 */

#pragma D option quiet

rubinius$target:::gc-begin
{
  in_use = arg1;
  obj_count = arg2;
}

rubinius$target:::gc-end
{
  printf("Young GC size/use: %d/%d (freed %d bytes) %d/%d objects (%d delta)\n", arg0, arg1, in_use - arg1, obj_count, arg2, arg2 - obj_count);
}

