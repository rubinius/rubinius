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

        ay = stats[:allocate_young]
        cy = stats[:collect_young]
        oc = cy[:objects_copied]
        op = cy[:objects_promoted]

        am = stats[:allocate_mature]
        cm = stats[:collect_mature]
        os = cm[:objects_seen]

        header = "\n%-#{col1}s%#{col2}s%#{col3}s\n"
        format = "%-#{col1}s%#{col2}s%#{col3}s\n"
        n_a    = "n/a"


        total = ay[:total] + cy[:total] + am[:total] + cm[:total]

        puts "\nGarbage collector stats:"

        young = "Young (%d)" % (Rubinius::RUBY_CONFIG["rbx.gc.lifetime"] || 6)
        printf header, "Stats \\ Generation", young, "Mature"

        puts   "-" * (col1 + col2 + col3)
        printf format, "Collections",       comma(cy[:timings]), comma(cm[:timings])
        printf format, "total time",        auto_time(cy[:total]), auto_time(cm[:total])
        printf format, " max",              auto_time(cy[:max]), auto_time(cm[:max])
        printf format, " min",              auto_time(cy[:min]), auto_time(cm[:min])
        printf format, " average",          auto_time(cy[:average]), auto_time(cm[:average])

        puts   "--"
        printf format, "objects copied/seen", comma(oc[:total]), comma(os[:total])
        printf format, " max",              comma(oc[:max]), comma(os[:max])
        printf format, " min",              comma(oc[:min]), comma(os[:min])
        printf format, " average",          comma(oc[:average].to_i), comma(os[:average].to_i)
        printf format, "bytes copied",      auto_bytes(cy[:bytes_copied]), n_a

        puts   "--"
        printf format, "objects promoted",  comma(op[:total]), n_a
        printf format, " max",              comma(op[:max]), n_a
        printf format, " min",              comma(op[:min]), n_a
        printf format, " average",          comma(op[:average].to_i), n_a
        printf format, "% of GC time",
               "(#{percentage(cy[:total], total)})", "(#{percentage(cm[:total], total)})"

        puts   "---"
        printf format, "Allocations",       comma(ay[:timings]), comma(am[:timings])
        printf format, "total time",        auto_time(ay[:total]), auto_time(am[:total])
        printf format, " max",              auto_time(ay[:max]), auto_time(am[:max])
        printf format, " min",              auto_time(ay[:min]), auto_time(am[:min])
        printf format, " average",          auto_time(ay[:average]), auto_time(am[:average])
        printf format, "bytes allocated",
               auto_bytes(ay[:bytes_allocated]), auto_bytes(am[:bytes_allocated])

        puts   "--"
        printf format, "large objects",     n_a, comma(stats[:large_objects])
        printf format, "% of GC time",
               "(#{percentage(ay[:total], total)})", "(#{percentage(am[:total], total)})"

        printf "\nTotal time spent in GC: %s (%s)\n\n",
               auto_time(total), percentage(total, stats[:clock])
      end
    end
  end
end
