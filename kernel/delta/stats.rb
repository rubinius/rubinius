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
      def comma(number)
        number.to_s.reverse.scan(/(?:\d*\.)?\d{1,3}-?/).join(',').reverse
      end

      def percentage(part, whole, units=:sec)
        case units
        when :sec
          part  = sec part
          whole = sec whole
        when :msec
          part  = msec part
          whole = msec whole
        end

        "%.1f%%" % (part * 100.0 / whole)
      end

      def auto_bytes(bytes)
        return bytes.to_s if bytes < KBYTES

        bytes /= KBYTES.to_f
        return "%.1fK" % bytes if bytes < KBYTES

        "%.1fM" % (bytes / KBYTES)
      end
    end

    class GC
      include Units

      def clear
        Ruby.primitive :vm_stats_gc_clear
        raise PrimitiveFailure, "Rubinius::Stats::GC.clear primitive failed"
      end

      def info
        Ruby.primitive :vm_stats_gc_info
        raise PrimitiveFailure, "Rubinius::Stats::GC.info primitive failed"
      end

      def show
        unless stats = info
          puts "\nNo GC stats information available. Build with 'rake build:stats'"
          return
        end

        col1, col2, col3 = 22, 15, 15

        cy = stats[:collect_young]
        ay = stats[:allocate_young]
        cm = stats[:collect_mature]
        am = stats[:allocate_mature]

        header  = "\n%-#{col1}s%#{col2}s%#{col3}s\n"
        format = "%-#{col1}s%#{col2}s%#{col3}s\n"

        total = ay[:total] + cy[:total] + am[:total] + cm[:total]

        puts "\nGarbage collector stats:"

        printf header, "Stats \\ Generation", "Young", "Mature"
        puts   "-" * (col1 + col2 + col3)
        printf format, "collections",     comma(cy[:timings]), comma(cm[:timings])
        printf format, "total",           auto_time(cy[:total]), auto_time(cm[:total])
        printf format, "max",             auto_time(cy[:max]), auto_time(cm[:max])
        printf format, "min",             auto_time(cy[:min]), auto_time(cm[:min])
        printf format, "average",         auto_time(cy[:average]), auto_time(cm[:average])
        printf format, "objects copied",  comma(cy[:objects_copied]), "n/a"
        printf format, "bytes copied",    auto_bytes(cy[:bytes_copied]), "n/a"
        printf format, "% of GC time",
               percentage(cy[:total], total), percentage(cm[:total], total)
        puts   "---"
        printf format, "allocations",     comma(ay[:timings]), comma(am[:timings])
        printf format, "total",           auto_time(ay[:total]), auto_time(am[:total])
        printf format, "max",             auto_time(ay[:max]), auto_time(am[:max])
        printf format, "min",             auto_time(ay[:min]), auto_time(am[:min])
        printf format, "average",         auto_time(ay[:average]), auto_time(am[:average])
        printf format, "bytes allocated",
               auto_bytes(ay[:bytes_allocated]), auto_bytes(am[:bytes_allocated])
        printf format, "% of GC time",
               percentage(ay[:total], total), percentage(am[:total], total)

        printf "\nTotal time spent in GC: %s (%s)\n\n",
               auto_time(total), percentage(total, stats[:clock])
      end
    end
  end
end
