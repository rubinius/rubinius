module Rubinius::Profiler

  ##
  # Interface to VM's instrumenting profiler.

  class Instrumenter
    attr_reader :profile

    SEC_PER_NS = 1.0e-9
    MS_PER_NS  = 1.0e-6

    # This is the set of Rubinius implementation classes to filter
    KERNEL_CLASSES = /^(Compiler|InstructionSet|Rubinius|InstructionSequence|CompiledMethod|Sexp|BlockEnvironment|AccessVariable|MethodContext|IncludedModule)/

    # If +filter+ is true, omit Rubinius implementation classes from the
      # profile. The core library classes are still shown. If +filter+ is
      # false, show all called methods.
    def initialize(filter = true)
      @filter = filter
    end

    def activate
      Ruby.primitive :vm_profiler_instrumenter_start
      raise PrimitiveFailure, "Profiler::Instrumenter#activate failed"
    end

    def terminate
      Ruby.primitive :vm_profiler_instrumenter_stop
      raise PrimitiveFailure, "Profiler::Instrumenter#terminate failed"
    end

    def start
      activate
    end

    def stop
      @profile = terminate
    end

    # Convenience method to profile snippets of code in a larger script or
    # program. Enables the profiler and yields to the given block.
    #
    #   pr = Rubinius::Profiler::Instrumenter.new
    #   pr.show { # do some work here }
    def show
      start
      yield if block_given?
      stop
      display
    end

    def display(out=STDOUT)
      unless @profile
        out.puts "No profiling data was available"
        return
      end

      total = 0.0
      data = @profile[:methods].values.map do |m|
        method_total = m[:total]
        callee_total = m[:leaves].inject(0) { |sum, leaf| sum + leaf.last }
        total += method_total

        [method_total, method_total - callee_total, m[:called], m[:name]]
      end

      data.sort! { |a, b| b.first <=> a.first }

      out.puts "  %   cumulative   self              self     total"
      out.puts " time   seconds   seconds    calls  ms/call  ms/call  name"
      out.puts "----------------------------------------------------------"

      data.each do |d|
        next if @filter and KERNEL_CLASSES =~ d[3]

        out.printf "%6.2f "  % (d[0] / total * 100.0)
        out.printf "%8.2f  " % (d[0] * SEC_PER_NS)
        out.printf "%8.2f "  % (d[1] * SEC_PER_NS)
        out.printf "%8d "    % d[2]
        out.printf "%8.2f "  % (d[1] * MS_PER_NS / d[2])
        out.printf "%8.2f  " % (d[0] * MS_PER_NS / d[2])
        out.printf "%s\n"    % d[3]
      end

      nil
    end
  end

  ##
  # Interface to VM's sampling profiler.

  class Sampler
    def initialize(freq=nil)
      @frequency = freq
      @frequency ||= ENV['PROFILE_FREQ'].to_i
      @frequency = 100 if @frequency == 0

      @call_graph = ENV['PROFILE_FULL']
    end

    def activate(hz)
      Ruby.primitive :sampler_activate
      raise PrimitiveFailure, "primitive failed"
    end

    def terminate
      Ruby.primitive :sampler_stop
      raise PrimitiveFailure, "primitive failed"
    end

    def start
      @start_clock = activate(@frequency)
      nil
    end

    def stop
      @results, @last_clock, @gc_cycles = terminate()
      nil
    end

    ##
    # Records call information.

    class Call
      attr_accessor :slices
      attr_accessor :descendants_slices
      attr_accessor :name
      attr_accessor :parents
      attr_accessor :children

      def initialize(name)
        @name = name
        @slices = 0
        @descendants_slices = 0

        @parents = Hash.new { |h,k| h[k] = 0 }
        @children = Hash.new { |h,k| h[k] = 0 }
      end

      def total_slices
        @slices + @descendants_slices
      end

      def count_parent(call)
        if call
          @parents[call] += 1
          call.children[self] += 1
        end
      end
    end

    def display(out=STDOUT)
      @total_slices = 0
      @calls = Hash.new { |h,k| h[k] = Call.new(k) }

      @results.each do |ent|
        next unless ent

        @total_slices += 1

        call = find_call(ent)
        call.slices += 1

        # skip context unwinding for a primitive
        next if ent.kind_of? Fixnum

        if @call_graph
          # count parents and children
          call.count_parent(find_call(ent.sender))

          # calc descendants
          seen_calls = { call => 1 }

          while true
            ent = ent.sender
            break unless ent

            c = find_call(ent)

            # unwind to the root, but count each call only once
            unless seen_calls[c]
              seen_calls[c] = 1

              c.descendants_slices += 1
            end
          end
        end
      end

      @calls["VM.garbage_collection"].slices = @gc_cycles

      out << "Total slices: #{@total_slices}, #{@last_clock - @start_clock} clocks\n\n"
      out << "=== FLAT PROFILE ===\n\n"
      out << " % time   slices   name\n"

      @calls.sort { |a, b| b[1].slices <=> a[1].slices }.each do |name, call|
        out.printf " %6.2f %8d    %s\n", percent(call.slices), call.slices, name
      end

      if @call_graph
        out << "\n=== CALL GRAPH ===\n\n"
        out << " % time   slices % self   slices  name\n"
        @calls.sort { |a, b| b[1].total_slices <=> a[1].total_slices }.each do |name, call|
          print_relatives(out, call.parents.sort { |a,b| a[1] <=> b[1] })

          out.printf " %6.2f %8d %6.2f %8d   %s\n",
            percent(call.total_slices), call.total_slices,
            percent(call.slices), call.slices,
            name

          print_relatives(out, call.children.sort { |a,b| b[1] <=> a[1] })

          out << "----------------------------------------------------------------------\n"
        end
      end
      nil
    end

    def context_name(entry)
      # a Fixnum means that a primitive was running
      if entry.kind_of? Fixnum
        "VM.primitive => #{Rubinius::Primitives[entry]}"
      else
        entry.normalized_name
      end
    end

    def find_call(entry)
      @calls[context_name(entry)]
    end

    def percent(slices)
      100.0 * slices / @total_slices
    end

    def print_relatives(out, rels)
      rels[0,5].each do |rel|
        out << "                                       #{rel[0].name} (#{rel[1]})\n"
      end
    end
  end

  ##
  # Displays Selector statistics.

  class Selectors
    def show_stats(range=30)

      count = Selector::ALL.size

      entries = Selector::ALL.values.map { |s| [s, s.receives] }

      entries.delete_if { |e| e.last < 10 }

      sort = entries.sort { |a,b| b.last <=> a.last }

      puts "\nTotal Selectors: #{count}"
      puts "Top #{range}, by receives:"
      puts "%-20s| %-20s| %s" % ["name", "receives", "send sites"]
      puts "=========================================================="
      sort[0,range].each do |entry|
        puts "%-20s| %-20d| %d" % [entry.first.name, entry.last, entry.first.send_sites.size]
      end
    end
  end

  ##
  # Displays SendSite statistics.

  class SendSites
    def show_stats(range=30)
      send_sites = Selector::ALL.values.inject([]) { |acc,s| acc.concat(s.send_sites) }
      count = send_sites.size

      send_sites.delete_if { |e| (e.hits + e.misses) < 10 }

      sort = send_sites.sort { |a,b| (b.hits + b.misses) <=> (a.hits + a.misses) }

      puts "\nTotal SendSites: #{count}"
      puts "Top #{range}, by sends:"
      puts "%-32s| %-18s | %-18s| %-10s| %s" % ["sender", "receiver class", "name", "hits", "misses"]
      puts "==================================================================================================="
      sort[0,range].each do |entry|
        mod = entry.sender.scope.module if entry.sender.scope
        sender = "#{mod}##{entry.sender.name}"
        puts "%-32s| %-18s | %-18s| %-10d| %d" % [sender, entry.recv_class, entry.name, entry.hits, entry.misses]
      end
    end
  end
end
