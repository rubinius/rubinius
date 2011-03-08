module Benchmark
  def compare(*reports)
    return if reports.size < 2

    iter = false
    sorted = reports.sort do |a,b|
      if a.respond_to? :ips
        iter = true
        b.ips <=> a.ips
      else
        a.runtime <=> b.runtime
      end
    end

    best = sorted.shift

    STDOUT.puts "\nComparison:"

    if iter
      STDOUT.printf "%20s: %10.1f i/s\n", best.label, best.ips
    else
      STDOUT.puts "#{best.rjust(20)}: #{best.runtime}s"
    end

    sorted.each do |report|
      name = report.label

      if iter
        x = (best.ips.to_f / report.ips.to_f)
        STDOUT.printf "%20s: %10.1f i/s - %.2fx slower\n", name, report.ips, x
      else
        x = "%.2f" % (report.ips.to_f / best.ips.to_f)
        STDOUT.puts "#{name.rjust(20)}: #{report.runtime}s - #{x}x slower"
      end
    end

    STDOUT.puts
  end

  module_function :compare
end
