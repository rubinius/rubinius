require 'core/build_config' unless defined?(Rubinius::BUILD_CONFIG)
require 'rubinius/configuration_variables'

Rubinius::ConfigurationVariables.define do |cv|

  cv.section "capi" do |s|
    s.vm_variable "global_flush", false,
      "Flush all CAPI handles at CAPI call boundaries"

    s.vm_variable "lock", false,
      "Lock around using CAPI methods"
  end

  cv.section "codedb" do |s|
    s.vm_variable "core.path", "",
      "Path for the Rubinius core library CodeDB"

    s.section "cache" do |e|
      e.vm_variable "enabled", false,
        "Cache code at runtime"
      e.vm_variable "path", "$TMPDIR/$PROGRAM_NAME-$USER-codedb",
        "Path for the CodeDB cache"
      e.vm_variable "size", 78643200,
        "Maximum size in bytes of the CodeDB cache"
      e.vm_variable "validate", false,
        "Check the CodeDB cache signature"
      e.vm_variable "purge", false,
        "Purge the CodeDB cache"
    end
  end

  cv.section "console" do |s|
    s.vm_variable "path", "$TMPDIR/$PROGRAM_NAME-$USER-console",
      "Path for Rubinius Console connection file"

    s.vm_variable "access", 0660,
      "Permissions on the Rubinius Console connection file"
  end

  cv.section "diagnostics" do |s|
    s.vm_variable "interval", 60000,
      "Number of milliseconds between aggregation of diagnostics"

    s.vm_variable "target", "none",
      "Location to send diagnostics: host:port, path"

    s.section "codedb" do |db|
      db.vm_variable "enabled", false,
        "Emit CodeDB diagnostics"
    end

    s.section "console" do |c|
      c.vm_variable "enabled", false,
        "Emit Console diagnostics"
    end

    s.section "collector" do |g|
      g.vm_variable "enabled", false,
        "Emit Garbage Collector diagnostics"
    end

    s.section "machine" do |m|
      m.vm_variable "enabled", false,
        "Emit Machine diagnostics"
    end

    s.section "memory" do |m|
      m.vm_variable "enabled", false,
        "Emit Memory diagnostics"
    end

    s.section "profiler" do |p|
      p.vm_variable "enabled", false,
        "Emit Profiler data"

      p.vm_variable "min_sample", 10,
        "Minimum number of samples for reporting"

      p.vm_variable "subprocess", false,
        "Enable profiling in subprocesses created by fork"
    end

    s.section "concurrency" do |c|
      c.vm_variable "enabled", false,
        "Emit concurrency diagnostics"
    end
  end

  cv.section "collector" do |s|
    s.vm_variable "honor_start", false,
      "Control whether or not GC.start is honored when called"

    s.vm_variable "third_region_threshold", (25 * 1024 * 1024),
      "The number of bytes allocated before the marksweep GC region is collected"

    s.vm_variable "malloc_threshold", (25 * 1024 * 1024),
      "How many bytes allocated by C extensions til the GC is run"
  end

  cv.section "jit" do |s|
    s.vm_variable "enabled", false,
      "Just-in-time (JIT) compile managed code to optimized code"

    s.vm_variable "inline", false,
      "Inline methods and funtions when JIT compiling"

    s.vm_variable "tier", 0,
      "Set max JIT tier to enable, 0: bytecode, 1: non-inlining, 2: inlining"
  end

  cv.section "log" do |s|
    s.vm_variable "target", "$TMPDIR/$PROGRAM_NAME-$USER.log",
      "Logging facility to use: 'syslog', 'console', or path"

    s.vm_variable "limit", (10 * 1024 * 1024),
      "The maximum size of the log file before log rotation is performed"

    s.vm_variable "archives", 5,
      "The number of prior logs that will be saved as '$(system.log).N.Z' zip files"

    s.vm_variable "access", 0600,
      "Permissions on the log file"

    s.vm_variable "level", "warn",
      "Logging level: fatal, error, warn, info, debug"

    s.vm_variable "config", true,
      "Log configuration options"

    s.vm_variable "lifetime", true,
      "Log events during the process lifetime"

    s.vm_variable "filter", /^core\/.*$/,
      "Filter paths matching pattern when logging events"

    s.vm_variable "exceptions", false,
      "Log exceptions when they are raised"

    s.section "collector" do |c|
      c.vm_variable "set", false,
        "Log when garbage collector is activated"
      c.vm_variable "run", false,
        "Log when garbage collector runs"
      c.vm_variable "terminal", false,
        "Log a separator line to the terminal when garbage collector runs"
    end

    s.section "fiber" do |f|
      f.vm_variable "lifetime", true,
        "Log events during the Fiber's lifetime"

      f.vm_variable "finalizer", false,
        "Log finalizer execution"

      f.vm_variable "filter", /^core\/.*$/,
        "Filter paths matching pattern when logging events"
    end

    s.section "thread" do |t|
      t.vm_variable "lifetime", true,
        "Log events during the Thread's lifetime"

      t.vm_variable "finalizer", false,
        "Log finalizer execution"

      t.vm_variable "filter", /^core\/.*$/,
        "Filter paths matching pattern when logging events"
    end

    s.section "jit" do |j|
      j.vm_variable "candidate", false,
        "Log when a function or method is considered for JIT"

      j.vm_variable "compile", false,
        "Log when a function or method is compiled and installed"

      j.vm_variable "inline", false,
        "Log when a function or method is inlined"
    end
  end

  cv.section "machine" do |s|
    s.section "fiber" do |f|
      f.vm_variable "stack_size", 512 * 1024,
        "The size in bytes of the Fiber's stack"
    end

    s.section "thread" do |t|
      t.vm_variable "stack_size", 4 * 1024 * 1024,
        "The size in bytes of the Thread's stack"
    end

    s.vm_variable "stack_cushion", 4096,
      "Size in bytes to reserve when checking stack overflow"

    s.section "call_site" do |cs|
      cs.vm_variable "cache.limit", 10,
        "Maximum number of caches at call sites"

      cs.vm_variable "eviction.limit", 25,
        "Maximum number of evictions before disabling caching at the call site"
    end

    s.section "unwind_site" do |u|
      u.vm_variable "limit", 3,
        "Maximum number of caches at unwind sites"
    end

    s.section "concurrent" do |c|
      c.section "update" do |u|
        u.vm_variable "log", false,
          "Log when T2 updates an object created by T1"

        u.vm_variable "raise", false,
          "Raise Rubinius::ConcurrentUpdateError when T2 updates an object created by T1"
      end
    end
  end

  cv.section "memory" do |s|
    s.vm_variable "large_object", (1024 * 1024),
      "The size (in bytes) of the large object threshold"

    s.vm_variable "autopack", true,
      "Set whether or not objects should be packed tightly in memory"
  end

  cv.vm_variable "config.print", 0,
    :as => "print_config",
    :description => "blank or 1: names and values, 2: description as well"

  cv.vm_variable "serial.debug", false,
    "Print out when the global serial increases"

  cv.section "tmp" do |t|
    t.vm_variable "path", "$TMPDIR",
      "Default temp/fallback directory for the process"
  end
end
