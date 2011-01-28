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

  end
end
