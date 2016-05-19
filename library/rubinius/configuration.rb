require 'core/build_config' unless defined?(Rubinius::BUILD_CONFIG)
require 'rubinius/configuration_variables'

Rubinius::ConfigurationVariables.define do |c|

  c.section "codedb" do |s|
    s.vm_variable "core.path", "$RUNTIME/core",
      "Path for the Rubinius core library CodeDB"
    s.vm_variable "cache.path", "$TMPDIR/$PROGRAM_NAME-$USER-codedb",
      "Path for the compiled code cache CodeDB"
  end

  c.section "memory" do |s|
    s.section "collection" do |c|
      c.vm_variable "log", false,
        "Log when collections are triggered and run"
    end
  end

  c.section "gc" do |s|
    s.vm_variable "young_bytes", (30 * 1024 * 1024),
      "The number of bytes the young generation of the GC should use"

    s.vm_variable "young_lifetime", 2,
      "How many young GC cycles an object lives before promotion"

    s.vm_variable "large_object", (1024 * 1024),
      "The size (in bytes) of the large object threshold"

    s.vm_variable "immix.concurrent", false,
      "Set whether we want the Immix mark phase to run concurrently"

    s.vm_variable "immix.debug", :bool,
      "Print out collection stats when the Immix collector finishes"

    s.vm_variable "honor_start", false,
      "Control whether or not GC.start is honored when called"

    s.vm_variable "autopack", true,
      "Set whether or not objects should be packed tightly in memory"

    s.vm_variable "marksweep_threshold", (25 * 1024 * 1024),
      "The number of bytes allocated before the marksweep GC region is collected"

    s.vm_variable "malloc_threshold", (25 * 1024 * 1024),
      "How many bytes allocated by C extensions til the GC is run"
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
    "The minimum number of nanoseconds a profiler node must have to be reported"

  c.section "machine" do |s|
    s.section "call_site" do |cs|
      cs.vm_variable "cache", true,
        "Cache executables at call sites"

      cs.vm_variable "limit", 3,
        "Maximum number of caches at call sites"
    end
  end

  c.section "system" do |s|
    s.vm_variable "tmp", "$TMPDIR",
      "Default temp/fallback directory for the process"

    s.section "console" do |c|
      c.vm_variable "path", "$TMPDIR/$PROGRAM_NAME-$USER-console",
        "Path for Rubinius Console connection file"

      c.vm_variable "access", 0660,
        "Permissions on the Rubinius Console connection file"
    end

    s.vm_variable "log", "$TMPDIR/$PROGRAM_NAME-$USER.log",
      "Logging facility to use: 'syslog', 'console', or path"

    s.section "log" do |l|
      l.vm_variable "limit", (10 * 1024 * 1024),
        "The maximum size of the log file before log rotation is performed"

      l.vm_variable "archives", 5,
        "The number of prior logs that will be saved as '$(system.log).N.Z' zip files"

      l.vm_variable "access", 0600,
        "Permissions on the log file"

      l.vm_variable "level", "warn",
        "Logging level: fatal, error, warn, info, debug"
    end

    s.section "metrics" do |m|
      m.vm_variable "interval", 10000,
        "Number of milliseconds between aggregation of VM metrics"

      m.vm_variable "target", "none",
        "Location to send metrics every interval: 'statsd', path"

      m.vm_variable "statsd.server", "localhost:8125",
        "The [host:]port of the StatsD server"

      m.vm_variable "statsd.prefix", "host.$nodename.$pid.app.rbx",
        "Prefix for StatsD metric names"
    end

    s.section "diagnostics" do |d|
      d.vm_variable "target", "none",
        "Location to send diagnostics: host:port, path"
    end

    s.section "profiler" do |p|
      p.vm_variable "target", "none",
        "Location to send profiler output: 'diagnostics', path"

      p.vm_variable "interval", 10000,
        "Report profiler results every N samples"

      p.vm_variable "subprocess", false,
        "Enable profiling in subprocesses created by fork"
    end
  end
end

