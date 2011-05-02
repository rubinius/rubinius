module Rubinius
  module Profiler

    ##
    # Interface to VM's instrumenting profiler.

    class Instrumenter
      include Stats::Units

      attr_reader :info, :options

      def self.available?
        Rubinius::Tooling.available?
      end

      def self.active?
        Rubinius::Tooling.active?
      end

      @loaded = false
      def self.load
        return if @loaded
        Rubinius::Tooling.load File.expand_path("../profiler_vm", __FILE__)
        @loaded = true
      end

      def initialize(options = {})
        Instrumenter.load

        @options = { :sort => :percent }
        set_options options
        set_options :full_report => true if Config["profiler.full_report"]
        set_options :graph => true if Config["profiler.graph"]

        if path = Config["profiler.json"]
          set_options :json => path
        end

        if name = Config["profiler.output"]
          set_options :output => "#{File.expand_path(name)}-#{$$}"
        end

        if Config['profiler.cumulative_percentage']
          set_options :cumulative_percentage => true
        end

        if Config['profiler.classes']
          set_options :classes => true
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
        @options.merge!(options)
      end

      def start
        Rubinius::Tooling.enable
      end

      def __stop__
        Rubinius::Tooling.disable
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

      def show(out=$stdout)
        unless self.class.available? and @info
          out.puts "No profiling data was available"
          return
        end

        begin
          if name = options[:output]
            out = File.open name, "w"
          end

          if options[:json]
            json options[:json]
          elsif options[:graph]
            graph out
          else
            flat out
          end
        ensure
          out.close if options[:output]
        end

        nil
      end

      def epilogue(out, size, calls)
        unless options[:full_report] or size < SHORT_LINES
          out.puts "\n#{comma(size-SHORT_LINES)} methods omitted"
        end
        out.puts "\n#{comma(size)} methods called a total of #{comma(calls)} times"
      end

      def flat(out)
        keys = @info.keys.sort

        keys.each do |t_id|
          thread_flat out, t_id, @info[t_id]
          puts
        end
      end

      def thread_flat(out, t_id, info)
        total_calls = 0
        total = 0.0

        all_selves = 0.0

        # Because the info is actually in a tree form and we to just see it
        # flat for each method, we need to go through and collect all the stats
        # for each unique method.

        info[:nodes].each do |n_id, data|
          sub = data[4].inject(0) { |a,n| a + info[:nodes][n][1] }

          meth = info[:methods][data[0]]
          if cur = meth[:edge_total]
            meth[:edge_total] = cur + sub
          else
            meth[:edge_total] = sub
          end
        end

        data = info[:methods].values.map do |m|
          cumulative   = m[:cumulative]
          method_total = m[:total]
          edges_total  = m[:edge_total]
          self_total   = method_total - edges_total
          called       = m[:called]
          total       += method_total
          total_calls += called

          all_selves += self_total

          name = m[:name]
          name = "#toplevel" if name == "<metaclass>#__script__ {}"
          [ 0,
            sec(cumulative),
            sec(self_total),
            called,
            msec(self_total) / called,
            msec(cumulative) / called,
            name ]
        end

        all_selves = sec(all_selves)

        if options[:cumulative_percentage]
          data.each do |d|
            d[0] = (d[1] / sec(info[:runtime])) * 100
          end
        else
          data.each do |d|
            d[0] = (d[2] / all_selves) * 100
          end
        end

        columns = sort_order
        data = data.sort_by do |row|
          columns.map {|col| row[col] }
        end.reverse

        out.puts "Thread #{t_id}: total running time: #{sec(info[:runtime])}s"
        out.puts ""
        out.puts "  %   cumulative   self                self     total"
        out.puts " time   seconds   seconds      calls  ms/call  ms/call  name"
        out.puts "------------------------------------------------------------"

        report = options[:full_report] ? data : data.first(SHORT_LINES)
        report.each do |d|
          out.printf " %6s", ("%.2f" % d[0])
          out.printf "%8.2f  %8.2f %10d %8.2f %8.2f  %s\n", *d.last(6)
        end

        epilogue out, data.size, total_calls

        if options[:classes]
          classes = Hash.new { |h,k| h[k] = 0.0 }

          data.each do |row|
            if m = /(.*)[#\.]/.match(row.last)
              classes[m[1]] += ((row[2] / all_selves) * 100)
            end
          end

          out.puts "\nUsage percentage by class:"
          sorted = classes.to_a.sort_by { |row| row[1] }.reverse
          sorted.each do |row|
            out.printf "%6s: %s\n", ("%.2f" % row[1]), row[0]
          end
        end
      end

      def json(path)
        File.open path, "w" do |f|
          t_final = @info.size - 1
          t_idx = 0

          f.puts "["

          @info.each do |t_id, info|
            f.puts "{"
            f.puts "  \"thread_id\": #{t_id},"
            f.puts "  \"runtime\": #{info[:runtime]},"
            f.puts "  \"total_nodes\": #{info[:total_nodes]},"
            roots = info[:roots].map { |x| x.to_s.dump }.join(',')
            f.puts "  \"roots\": [ #{roots} ],"
            f.puts "  \"nodes\": {"
            idx = 0
            final = info[:nodes].size - 1

            info[:nodes].each do |n_id, data|
              f.puts "    \"#{n_id}\": {"
              f.puts "      \"method\": #{data[0]}, \"total\": #{data[1]}, \"called\": #{data[2]},"
              f.puts "      \"total_nodes\": #{data[3]}, \"sub_nodes\": [ #{data[4].join(', ')} ]"
              if idx == final
                f.puts "    }"
              else
                f.puts "    },"
              end
              idx += 1
            end

            f.puts "  },"
            f.puts "  \"methods\": {"

            idx = 0
            final = info[:methods].size - 1
            info[:methods].each do |m_id, m|
              f.puts "    \"#{m_id}\": {"
              f.puts "      \"name\": \"#{m[:name]}\", \"file\": \"#{m[:file]}\", \"line\": #{m[:line] || 0},"
              f.puts "      \"cumulative\": #{m[:cumulative]}, \"total\": #{m[:total]}, \"called\": #{m[:called]}"
              if idx == final
                f.puts "    }"
              else
                f.puts "    },"
              end
              idx += 1
            end

            f.puts "  }"

            if t_idx == t_final
              f.puts "}"
            else
              f.puts "},"
            end

            t_idx += 1
          end
          f.puts "]"
        end

        puts "Wrote JSON to: #{path}"
      end

      def graph(out)
        keys = @info.keys.sort

        keys.each do |t_id|
          thread_graph out, t_id, @info[t_id]
          out.puts
        end
      end

      # Prints an entry for each method, along with the method's callers and
      # the methods called. The entry is delimited by the dashed lines. The
      # line for the method itself is called the "primary" line. The callers
      # are printed above the primary line and the methods called are printed
      # below.
      def thread_graph(out, t_id, info)
        total_calls = 0
        run_total = 0.0

        data = info[:nodes]

        methods = info[:methods]

        run_total = info[:runtime].to_f

        all_callers = Hash.new { |h,k| h[k] = [] }

        data.each do |n_id, n_data|
          n_data[4].each do |sub|
            all_callers[sub] << n_id
          end
        end

        indexes = data.keys.sort do |a, b|
          data[b][1] <=> data[a][1]
        end

        indexes = indexes.first(SHORT_LINES) unless options[:full_report]

        shown_indexes = {}

        indexes.each_with_index do |id, index|
          shown_indexes[id] = index + 1
        end

        out.puts "===== Thread #{t_id} ====="
        out.puts "Total running time: #{sec(info[:runtime])}s"
        out.puts "index  % time     self  children         called       name"
        out.puts "----------------------------------------------------------"

        primary   = "%-7s%6s %8.2f %9.2f   %8d           %s [%d]\n"
        secondary = "              %8.2f %9.2f   %8d               %s%s\n"

        indexes.each do |id|
          m_id, total, called, tn, sub_nodes = data[id]

          # The idea is to report information about caller as a ratio of the
          # time it called method.
          #

          callers = all_callers[id].sort_by do |c_id|
            clr = data[c_id]

            clr[total]
          end

          callers = callers.first(10) unless options[:full_report]

          callers.each do |c_id|
            clr_m_id, clr_total, clr_called, clr_tn, clr_sub = data[c_id]

            sub_total = clr_sub.inject(0) { |a,s| a + data[s][1] }

            self_total = clr_total - sub_total
            out.printf(secondary, sec(self_total),
                                  sec(sub_total),
                                  clr_called,
                                  methods[clr_m_id][:name],
                                  graph_method_index(shown_indexes[c_id]))
          end

          # Now the primary line.

          children = sub_nodes.inject(0) { |a,s| a + data[s][1] }
          # children = method[:cumulative] * (method[:edges_total].to_f / method[:total])

          self_total = total - children
          out.printf primary, ("[%d]" % shown_indexes[id]),
                              percentage(total, run_total, 1, nil),
                              sec(self_total),
                              sec(children),
                              called,
                              methods[m_id][:name],
                              shown_indexes[id]

          # Same as caller, the idea is to report information about callee methods
          # as a ratio of the time it was called from method.
          #

          edges = sub_nodes.sort_by do |e_id|
            if edge = data[e_id]
              edge[1]
            else
              0.0
            end
          end

          edges = edges.last(10) unless options[:full_report]
          # method[:edges] = method[:edges].first(10) unless options[:full_report]

          edges.reverse_each do |e_id|
            c_m_id, c_total, c_called, c_tn, c_sub_nodes = data[e_id]

            grandchildren = c_sub_nodes.inject(0) { |a,s| a + data[s][1] }
            grandchildren = 0 if grandchildren < 0

            self_total = c_total - grandchildren
            out.printf secondary, sec(self_total),
                                  sec(grandchildren),
                                  c_called,
                                  methods[c_m_id][:name],
                                  graph_method_index(shown_indexes[e_id])
          end

          out.puts "-------------------------------------------------------"
        end

        epilogue out, data.size, total_calls
      end

      def graph_method_index(index)
        index ? " [#{index}]" : ""
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
  end
end
