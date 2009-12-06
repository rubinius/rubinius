require 'thread'

module Rubinius
  # A debugger, providing a CLI for debugging Ruby code running under Rubinius.
  # Takes advantage of Rubinius's support for full-speed debugging, via the
  # +Breakpoint+ and +BreakpointTracker+ classes.
  class Debugger
    @instance = nil
    @semaphore = Mutex.new

    # Only a single Debugger instance should be created
    def self.new
      super
    end
    self.private_class_method :new

    # Returns the singleton instance of the +Debugger+.
    def self.instance
      @semaphore.synchronize do
        @instance ||= new()
      end
    end

    def self.__instance__
      @instance
    end

    def self.__clear_instance
      @semaphore.synchronize do
        @instance = nil
      end
    end

    def handle_breakpoints
      until @quit do
        begin
          @debug_thread = @breakpoint_tracker.wait_for_breakpoint
        rescue Exception => e
          # An exception has occurred in the breakpoint or debugger code
          STDERR.puts "An exception occured while processing a breakpoint:"
          STDERR.puts e.to_s
          STDERR.puts e.awesome_backtrace
        end
        @breakpoint_tracker.wake_target(@debug_thread) unless @quit  # defer wake until we cleanup
      end

      begin
        # Release singleton, since our loop thread is exiting
        Debugger.__clear_instance

        # Remove all remaining breakpoints
        @breakpoint_tracker.clear_breakpoints

        # De-register debugger on the global debug channel
        Rubinius::VM.debug_channel = nil

        if @debug_thread
          # @debug_thread will be nil if debugger was quit from other than this thread
          @breakpoint_tracker.wake_target(@debug_thread)
        end
        @breakpoint_tracker.release_waiting_threads
      rescue Exception => e
        # An exception has occurred in the breakpoint or debugger code
        STDERR.puts "An exception occured while exiting the debugger:"
        STDERR.puts e.to_s
        STDERR.puts e.awesome_backtrace
      end

    end

    # Initializes a new +Debugger+ instance
    def initialize
      @breakpoint_tracker = BreakpointTracker.new do |thread, ctxt, bp|
        activate_debugger thread, ctxt, bp
      end

      # Register this debugger as the default debug channel listener
      Rubinius::VM.debug_channel = @breakpoint_tracker.debug_channel

      @quit = false
      @breakpoint_listener = Thread.new { handle_breakpoints }
      Thread.pass until waiting_for_breakpoint?
    end

    # Sets a breakpoint on a +CompiledMethod+ at the specified address.
    def set_breakpoint(cm, ip, condition=nil)
      @breakpoint_tracker.on(cm, :ip => ip, :condition => condition)
    end

    # Removes the breakpoint(s) specified.
    # +bp+ may be either a single Breakpoint instance or id, or an Array of
    # Breakpoint instances or ids.
    def remove_breakpoint(bp)
      if bp.kind_of? Array
        removed = []
        bp.each do |bp|
          removed << @breakpoint_tracker.remove_breakpoint(bp)
        end
        removed
      else
        @breakpoint_tracker.remove_breakpoint(bp)
      end
    end

    # Temporarily removes a breakpoint, but does not delete it
    def disable_breakpoint(bp)
      bp.disable
    end

    # Re-installs a disabled breakpoint
    def enable_breakpoint(bp)
      bp.enable
    end

    def step(selector)
      @breakpoint_tracker.step(selector)
    end

    # Clears all breakpoints
    def clear_breakpoints
      @breakpoint_tracker.clear_breakpoints
    end

    # Returns details of all breakpoints that are being managed by the debugger.
    # Note: This excludes transitory step breakpoints.
    def breakpoints
      @breakpoint_tracker.global_breakpoints
    end

    # Returns the breakpoint for the specified compiled method and IP
    def get_breakpoint(cm, ip)
      @breakpoint_tracker.get_breakpoint(cm, ip)
    end

    # True if the debugger is sleeping, i.e. waiting for a breakpoint to be hit.
    def waiting_for_breakpoint?
      @breakpoint_listener.status == 'sleep'
    end

    # Sets the quit flag to true, so that the debugger shuts down.
    def quit!
      @quit = true

      # If quit! is called from other than a command, we need to interrupt the
      # breakpoint listener thread
      unless @debug_thread
        @breakpoint_tracker.debug_channel.send nil
        @breakpoint_listener.join
      end
    end

    # Returns true if the debugger is shutting down
    def quit?
      @quit
    end

    # The interface used to interact with the debugger. A debugger interface must
    # implement a #process_commands method taking the following arguments:
    # - A reference to the Debugger instance
    # - The thread that has hit the breakpoint
    # - The context in which the breakpoint was hit
    # - An array of Breakpoint instances representing each of the breakpoints that
    #   were triggered at the current breakpoint location
    attr_accessor :interface

    # Activates the debugger after a breakpoint has been hit, and responds to
    # debgging commands until a continue command is recevied.
    def activate_debugger(thread, ctxt, bp_list)
      @debug_thread = thread
      @interface.at_breakpoint(self, thread, ctxt, bp_list)
    end

    # Retrieves the source code for the specified file, if it exists.
    # The last source listing is cached, since it is common to list the source
    # for a file multiple times.
    def source_for(file)
      return @last_lines if file == @last_file

      @last_file, @last_lines = file, nil
      if File.exists?(file)
        @last_lines = File.readlines(file)
      end
      @last_lines
    end

    # Returns the decoded instruction sequence for the specified CompiledMethod.
    # The last decode listing is cached, since it is common to list the bytecode
    # for a method multiple times.
    def asm_for(cm)
      return @last_asm if cm == @last_cm

      @last_cm, @last_asm = cm, cm.decode
      @last_asm
    end
  end
end

