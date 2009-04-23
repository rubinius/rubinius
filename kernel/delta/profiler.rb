# depends on: stats.rb

module Rubinius
  module Profiler

    ##
    # Interface to VM's instrumenting profiler.

    class Instrumenter
      include Stats::Units

      attr_reader :info, :options

      def self.available?
        Ruby.primitive :vm_profiler_instrumenter_available_p
        raise PrimitiveFailure, "Profiler::Instrumenter.available? failed"
      end

      def self.active?
        Ruby.primitive :vm_profiler_instrumenter_active_p
        raise PrimitiveFailure, "Profiler::Instrumenter.active? failed"
      end

      def initialize(options = {})
        self.set_options options
        if RUBY_CONFIG["rbx.profiler.full_report"]
          self.set_options :full_report => true
        end
      end

      # Set options for profiler output. Presently, the only option
      # is :sort. It takes a single symbol or an array of symbols to
      # specify the column(s) to sort by. The recognized symbols are:
      #
      #   Symbol            Profiler heading
      #   :percent          % time
      #   :total_seconds    cumulative seconds
      #   :self_seconds     self seconds
      #   :calls            calls
      #   :self_ms          self ms/call
      #   :total_ms         total ms/call
      #   :name             name
      #
      # @todo Add options for GC allocation counts
      def set_options(options)
        @options ||= { :sort => :percent }
        @options.merge!(options)
      end

      def start
        Ruby.primitive :vm_profiler_instrumenter_start
        raise PrimitiveFailure, "Profiler::Instrumenter#start failed"
      end

      def __stop__
        Ruby.primitive :vm_profiler_instrumenter_stop
        raise PrimitiveFailure, "Profiler::Instrumenter#stop failed"
      end

      def stop
        @info = __stop__
      end

      # Convenience method to profile snippets of code in a larger script or
      # program. Enables the profiler and yields to the given block.
      #
      #   pr = Rubinius::Profiler::Instrumenter.new
      #   pr.profile { # do some work here }
      def profile(display = true)
        start
        yield if block_given?
        stop
        show if display
        @info
      end

      SHORT_LINES = 45

      def show(out=STDOUT)
        unless self.class.available? and @info
          out.puts "No profiling data was available"
          return
        end

        total_calls = 0
        total = 0.0

        data = @info[:methods].values.map do |m|
          cumulative   = m[:cumulative]
          method_total = m[:total]
          edges_total  = m[:edges].inject(0) { |sum, edge| sum + edge.last }
          self_total   = method_total - edges_total
          called       = m[:called]
          total       += method_total
          total_calls += called

          name = m[:name]
          name = "#toplevel" if name == "<metaclass>#__script__ {}"
          [ method_total,
            sec(cumulative),
            sec(self_total),
            called,
            msec(self_total) / called,
            msec(cumulative) / called,
            name ]
        end

        columns = sort_order
        data.sort! do |a, b|
          columns.each do |c|
            cmp = b[c] <=> a[c]
            return cmp if cmp != 0
          end
          0
        end

        out.puts "  %   cumulative   self                self     total"
        out.puts " time   seconds   seconds      calls  ms/call  ms/call  name"
        out.puts "------------------------------------------------------------"

        report = options[:full_report] ? data : data.first(SHORT_LINES)
        report.each do |d|
          out.printf "%6s ", percentage(d.first, total, 2, nil)
          out.printf "%8.2f  %8.2f %10d %8.2f %8.2f  %s\n", *d.last(6)
        end

        puts "\n#{comma(data.size-SHORT_LINES)} omitted" unless options[:full_report]
        puts "\n#{comma(data.size)} methods called a total of #{comma(total_calls)} times"

        nil
      end

      HEADER_INDEX = {
        :percent       => 0,
        :total_seconds => 1,
        :self_seconds  => 2,
        :calls         => 3,
        :self_ms       => 4,
        :total_ms      => 5,
        :name          => 6

      }

      def sort_order
        # call to_i so if unrecognized symbol is passed, column will be percent
        Array(@options[:sort]).map { |header| HEADER_INDEX[header].to_i }
      end
    end

    ##
    # Interface to VM's sampling profiler.
    #
    # @todo needs to be implemented in the VM.

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
          "VM.primitive => #{Primitives[entry]}"
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
end
