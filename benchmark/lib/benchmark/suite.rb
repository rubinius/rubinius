require 'benchmark'

module Benchmark
  class Suite
    def self.current
      @current
    end

    def self.create
      if block_given?
        old = @current

        begin
          s = new
          @current = s
          yield s
          return s
        ensure
          @current = old
        end
      else
        @current = new
      end
    end

    class SimpleReport
      def initialize
        @start = nil
        @end = nil
      end

      attr_reader :start, :end
    end

    def initialize
      @report = nil
      @reports = {}
      @order = []
      @quiet = false
    end

    attr_reader :reports

    def quiet!
      @quiet = true
    end

    def quiet?
      @quiet
    end

    def warming(label, sec)
      return unless @verbose
      print "#{label.rjust(20)} warmup: #{sec}s"
    end

    def warmup_stats(time, cycles)
      return unless @verbose
      print " time=#{time}us, cycles=#{cycles}."
    end

    def running(label, sec)
      return unless @verbose
      puts " running: #{sec}s..."
    end

    def add_report(rep, location)
      if @report
        @report << rep
      else
        @report = [rep]
      end

      @report_location = location
    end

    def run(file)
      start = Time.now

      begin
        load file
      rescue Exception => e
        puts "\nError in #{file}:"
        if e.respond_to? :render
          e.render(STDERR)
        else
          puts e.backtrace
        end
        return
      end

      fin = Time.now

      if @report
        @reports[file] = @report
        @report = nil
      else
        @reports[file] = SimpleReport.new(start, fin)
      end

      @order << file
    end

    def display
      if @report
        file = @report_location ? @report_location.split(":").first : "<unknown.rb>"
        @order << file
        @reports[file] = [@report]
      end

      @order.each do |file|
        puts "#{file}:"
        reports = @reports[file]

        if reports.empty?
          puts "  NO REPORTS FOUND"
        else
          reports.each do |rep|
            puts "  #{rep}"
          end
        end
      end
    end
  end
end
