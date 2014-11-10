require 'rubinius/build_config'
require 'rubinius/configuration_variables'

Rubinius::ConfigurationVariables.define do |c|

  c.section "gc" do |s|
    s.vm_variable "young_initial_bytes", 3145728,
      "The initial number of bytes the young generation of the GC should use"

    s.vm_variable "young_max_bytes", 5 * 3145728,
      "The maximum number of bytes the young generation of the GC should use"

    s.vm_variable "young_autotune_size", true,
      "Set whether or not the young GC should autotune the size"

    s.vm_variable "young_autotune_factor", 8,
      "Set the young GC size autotune factor. This is the denominator of the fraction of total memory used for young GC"

    s.vm_variable "young_lifetime", 2,
      "How many young GC cycles an object lives before promotion"

    s.vm_variable "young_autotune_lifetime", true,
      "Set whether or not the young GC should adjust promotion age for performance"

    s.vm_variable "large_object", (50 * 1024),
      "The size (in bytes) of the large object threshold"

    s.vm_variable "show", :bool,
      "Display information whenever the GC runs"

    s.vm_variable "noisy", :bool,
      "Beep whenever the GC runs (once for young, twice for mature). Requires gc.show"

    s.vm_variable "immix.concurrent", true,
      "Set whether we want the Immix mark phase to run concurrently"

    s.vm_variable "immix.debug", :bool,
      "Print out collection stats when the Immix collector finishes"

    s.vm_variable "honor_start", false,
      "Control whether or not GC.start is honored when called"

    s.vm_variable "autopack", true,
      "Set whether or not objects should be backed tightly in memory"

    s.vm_variable "marksweep_threshold", (10 * 1024 * 1024),
      "The number of bytes allocated before the marksweep GC region is collected"

    s.vm_variable "malloc_threshold", 104857600,
      "How many bytes allocated by C extensions til the GC is run"
  end

  c.section "jit" do |s|
    s.vm_variable "dump_code", 0,
      "1 == show simple IR, 2 == show optimized IR, 4 == show machine code"

    s.section "threshold" do |t|
      t.vm_variable "compile", 32000,
        "Number of method calls that trigger the JIT compiler"

      t.vm_variable "inline", 8000,
        "Number of method calls that trigger inlining the method"
    end

    s.section "limit" do |l|
      l.vm_variable "deoptimize", 500,
        "Number of times the JIT assumptions are false before a method is deoptimized"

      l.vm_variable "overflow", 5000,
        "Number of times the inline cache overflows before emitting a send from the JIT to call the method"

      l.vm_variable "method_size", 5000,
        "The maximum size of a method that will be JIT compiled"

      l.vm_variable "search", 5,
        "The maximum depth to search for an inlininging target"

      l.section "inline" do |i|
        i.vm_variable "method", 200,
          "The maximum size of a method to be considered for inlining"

        i.vm_variable "small", 2000,
          "The maximum total inlining budget size for a small method"

        i.vm_variable "normal", 3000,
          "The maximum total inlining budget for a normal method"

        i.vm_variable "large", 5000,
          "The maximum total inlining budget for a large method"
      end
    end

    s.vm_variable "show", false,
      :as => "jit_show_compiling",
      :description => "Print out a status message when the JIT is operating"

    s.vm_variable "profile", false,
      "The JIT will emit code to be sure JITd methods can be profile"

    s.section "inline" do |i|
      i.vm_variable "generic", true, "Have the JIT inline generic methods"

      i.vm_variable "blocks", true,
        "Have the JIT try and inline methods and their literal blocks"

      i.vm_variable "debug", false,
        "Have the JIT print out information about inlining"
    end

    s.vm_variable "log", :string,
      "Send JIT debugging output to this file rather than stdout"

    s.vm_variable "debug", false,
      "Have the JIT print debugging information"

    s.vm_variable "sync", false,
      "Wait for the JIT to finish compiling each method"

    s.vm_variable "uncommon.print", false,
      "Print out information on when methods are deoptimized due to uncommon traps"

    s.vm_variable "removal.print", false,
      "Print out whenever the JIT is removing unused code"

    s.vm_variable "check_debugging", false,
      "Allow JITd methods to deoptimize if there is a debugging request"

    s.vm_variable "type.optz", true,
      "Enable optimizations based on type flow"
  end

  c.section "fiber" do |s|
    s.vm_variable "stacks", 10,
      "The number of stacks in each Threads stack pool"

    s.vm_variable "stack_size", 512 * 1024,
      "The size of each stack"
  end

  c.vm_variable "tool", :string,
    :as => "tool_to_load",
    :description => "Load a VM tool from a shared library"

  c.section "capi" do |s|
    s.vm_variable "global_flush", false,
      "Flush all CAPI handles at CAPI call boundaries"

    s.vm_variable "lock", false,
      "Lock around using CAPI methods"
  end

  c.vm_variable "int", false,
    :as => "jit_disabled",
    :description => "Force the JIT to never turn on"

  c.vm_variable "config.print", 0,
    :as => "print_config",
    :description => "blank or 1 == names and values, 2 == description as well"

  c.vm_variable "ic.debug", false,
    "Print out when inline caches are reset"

  c.vm_variable "serial.debug", false,
    "Print out when the global serial increases"

  c.vm_variable "allocation_tracking", false,
    "Enable allocation tracking for new objects"

  c.vm_variable "profile", false,
    "Configure the system to profile ruby code"

  c.vm_variable "profiler.threshold", 1000000,
    "The mininum number of nanoseconds a profiler node must have to be reported"

  c.vm_variable "system.tmp", "$TMPDIR",
    "Default temp/fallback directory for the process"

  c.section "system" do |s|
    s.vm_variable "fsapi.path", "$TMPDIR",
      "Base directory of the Rubinius File System API files"

    s.vm_variable "fsapi.access", 0750,
      "Permissions on the Rubinius File System API directory"

    s.vm_variable "console.access", 0660,
      "Permissions on the Rubinius Console files"

    s.vm_variable "log", "/var/log/$PROGRAM_NAME.log",
      "Logging facility to use: 'syslog', 'console', or path"

    s.vm_variable "log.level", "warn",
      "Logging level: fatal, error, warn, info, debug"

    s.vm_variable "metrics.interval", 10000,
      "Number of milliseconds between aggregation of VM metrics"

    s.vm_variable "metrics.target", "none",
      "Location to send metrics every interval: 'statsd', 'disk'"

    s.vm_variable "metrics.statsd.server", "localhost:8125",
      "The [host:]port of the StatsD server"

    s.vm_variable "metrics.statsd.prefix", "host.$nodename.app.rbx",
      "Prefix for StatsD metric names"
  end
end

