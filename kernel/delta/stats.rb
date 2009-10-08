module Rubinius
  module Stats
    module Units
      # Time unit factors, where one second is
      # 10^9 nanoseconds (ns)
      # 10^6 microseconds (us)
      # 10^3 milliseconds (ms)

      SEC_PER_NS  = 1.0e-9
      MSEC_PER_NS = 1.0e-6
      USEC_PER_NS = 1.0e-3

      KBYTES = 1024

      def usec(ns)
        ns * USEC_PER_NS
      end

      def msec(ns)
        ns * MSEC_PER_NS
      end

      def sec(ns)
        ns * SEC_PER_NS
      end

      def auto_time(time)
        return "0" if time == 0

        elapsed = sec time
        return "%.2fs" % elapsed if elapsed > 1.0

        elapsed = msec time
        return "%.2fm" % elapsed if elapsed > 1.0

        elapsed = usec time
        return "%.2fu" % elapsed if elapsed > 1.0

        "%dn" % time
      end

      # Thanks Ruby Quiz #113
      def comma(number, digits=2)
        if number.is_a? Float
          str = "%.#{digits}f" % number
        else
          str = number.to_s
        end

        str.reverse.scan(/(?:\d*\.)?\d{1,3}-?/).join(',').reverse
      end

      def percentage(part, whole, digits=1, percent='%%')
        "%.#{digits}f#{percent}" % (part * 100.0 / whole)
      end

      def auto_bytes(bytes)
        return bytes.to_s if bytes < KBYTES

        bytes /= KBYTES.to_f
        return "%.1fK" % bytes if bytes < KBYTES

        "%.1fM" % (bytes / KBYTES)
      end
    end

    module Printer
      def columns(*columns)
        @width = columns.inject(0) { |sum, x| sum + x }

        text_column = "%%-%ds" % columns.shift
        @value_format = "#{text_column}%#{columns[1]}s\n"

        format = columns.map { |c| "%%%ds" % c }.join
        @stats_format = "#{text_column}#{format}\n"

        @header_format = "\n" << @stats_format
      end

      def headings(*headings)
        @headings = headings
      end

      def heading(leader)
        printf @header_format, leader, *@headings
        separator
      end

      def separator(limit=nil)
        width = limit || @width
        puts "-" * width
      end

      def string(metric, string)
        printf @value_format, metric, string
      end

      def value(metric, value, meth=:comma)
        printf @value_format, metric, send(meth, value)
      end

      def statistics(metric, data, meth=:comma)
        values = [data[:total], data[:max], data[:min], data[:average]]
        printf @stats_format, metric, *values.map { |d| send meth, d }
      end
    end

    class GC
      include Units
      include Printer

      def clear
        Ruby.primitive :vm_stats_gc_clear
        raise PrimitiveFailure, "Rubinius::Stats::GC.clear primitive failed"
      end

      def info
        Ruby.primitive :vm_stats_gc_info
        raise PrimitiveFailure, "Rubinius::Stats::GC.info primitive failed"
      end

      def show
        unless data = info
          puts "\nNo GC stats information available. Build with 'rake build:stats'"
          return
        end

        columns 25, 14, 14, 14, 14
        headings "total", "max", "min", "average"

        collect_young  = data[:collect_young]
        allocate_young = data[:allocate_young]

        collect_mature  = data[:collect_mature]
        allocate_mature = data[:allocate_mature]

        total = allocate_young[:total] + collect_young[:total] +
                allocate_mature[:total] + collect_mature[:total]

        puts "\nGarbage collector stats:"

        # TODO: make all configured values, even defaults, visible in config
        young = "Young (%d)" % (Rubinius::Config["rbx.gc.lifetime"] || 6)
        heading young

        value      "Collections",       collect_young[:timings]
        statistics " times",            collect_young, :auto_time
        statistics " objects promoted", collect_young[:objects_promoted]
        statistics " objects copied",   collect_young[:objects_copied]
        statistics " bytes copied",     collect_young[:bytes_copied], :auto_bytes

        puts       "Lifetimes"
        collect_young[:lifetimes].each_with_index do |lifetime, index|
          value    " #{index}", lifetime
        end
        string     "% of GC time",      "(#{percentage(collect_young[:total], total)})"

        separator  2
        value      "Allocations",       allocate_young[:timings]
        statistics " times",            allocate_young, :auto_time
        value      " bytes allocated",  allocate_young[:bytes_allocated], :auto_bytes
        object_types allocate_young[:object_types]
        string     "% of GC time",      "(#{percentage(allocate_young[:total], total)})"


        heading    "Mature"
        value      "Collections",       collect_mature[:timings]
        statistics " times",            collect_mature, :auto_time
        string     "% of GC time",      "(#{percentage(collect_mature[:total], total)})"

        separator  2
        value      "Allocations",       allocate_mature[:timings]
        statistics " times",            allocate_mature, :auto_time
        value      " chunks added",     allocate_mature[:chunks_added]
        value      " large objects",    allocate_mature[:large_objects]
        object_types allocate_mature[:object_types]
        string     "% of GC time",      "(#{percentage(allocate_mature[:total], total)})"

        printf "\nTotal time spent in GC: %s (%s)\n\n",
               auto_time(total), percentage(total, data[:clock])
      end

      def object_types(data)
        return if data.empty? || !Rubinius::Config["rbx.gc_stats.object_types"]

        total = 0
        puts "Object types"
        data.each_with_index do |count, type|
          next if count == 0
          value " #{object_type type}", count
          total += count
        end
        value " Total types", total
      end
      private :object_types

      def object_type(type)
        unless @types
          # TODO: make an interface to these available, potentially as part of
          # making the compiler aware of these types.
          object_types = File.dirname(__FILE__) + '/../../vm/gen/object_types.hpp'
          if File.exists? object_types
            @types = IO.read(object_types).scan(/\b(\w*)Type/).flatten
          else
            @types = []
          end
        end

        @types[type]
      end
      private :object_type
    end
  end
end
