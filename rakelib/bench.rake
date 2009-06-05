require 'rakelib/git'

# Some tasks to automate running the Ruby Benchmark Suite (RBS)
# on Rubinius. The results are output to YAML and processed by
# the :results task into graphs on a webpage.
#
# The :run task does not depend on the :update tasks so the
# updates are done manually to ensure stability.

BASEDIR         = File.expand_path(File.dirname(__FILE__) + "/..")
MONITOR         = BASEDIR + "/benchmark/utils/monitor.rb"
RUNNER          = BASEDIR + "/benchmark/utils/bench.rb"
RBS_DIR         = BASEDIR + "/benchmark/rbs"
RESULTS_DIR     = BASEDIR + "/benchmark/results"
RBS_RESULTS_DIR = RESULTS_DIR + "/rbs"
WEB_DIR         = RESULTS_DIR + "/web"

ITERATIONS      = (ENV['ITERATIONS'] || 5).to_i
TIMEOUT         = (ENV['TIMEOUT'] || 300).to_i
VM              = ENV['VM'] || "#{BASEDIR}/bin/rbx"

def command(name)
  "ruby #{MONITOR} #{TIMEOUT} '#{VM}' #{RUNNER} #{name} #{ITERATIONS} #{report}"
end

# Cache the name so it is only generated once during an invocation.
# Eliminates having to save the name and pass it around.
def report
  vm = File.basename VM.split.first
  @report ||= "#{RBS_RESULTS_DIR}/RBS-#{vm}-#{Time.now.strftime "%d-%m-%Y-%H%M"}.yaml"
end

def report_name
  report[(BASEDIR.size+1)..-1]
end

desc "Run the RBS benchmarks"
task :bench => 'bench:run'

namespace :bench do
  desc "Plot the RBS benchmark results"
  task :results => :setup do
  end

  desc "Generate a CSV file of results"
  task :to_csv => :setup do
    require 'yaml'

    field = ENV['FIELD'] || "min"
    unless ["max", "min", "median", "mean"].include?(field)
      raise "FIELD must be one of max, min, median, mean"
    end

    dir = ENV['RESULTS'] || RBS_RESULTS_DIR

    header = ["Benchmark file", "input"]
    data   = Hash.new { |h,k| h[k] = {} }
    status = Hash.new { |h,k| h[k] = {} }

    Dir[dir + "/**/*.yaml"].sort.each do |name|
      system = File.basename name, ".yaml"
      header << system

      File.open name, "r" do |file|
        YAML.load_documents file do |doc|
          bench_name = File.basename(doc["name"], ".rb")
          status[bench_name][system] ||= doc["status"]

          next unless doc.key? field

          bench = [bench_name, doc["parameter"]]
          data[bench]["input"] = doc["parameter"]
          data[bench][system] = doc[field]
        end
      end
    end

    csv_report = "#{dir}/RBS-#{Time.now.strftime "%d-%m-%Y-%H%M"}.csv"
    File.open csv_report, "w" do |file|
      file.puts(header.map { |h| h.inspect }.join(","))
      header.shift

      data.keys.sort.each do |key|
        file.print key.first.inspect, ","
        line = header.map do |h|
          (data[key][h] || status[key.first][h]).inspect
        end
        file.puts line.join(",")
      end
    end
  end

  # Not public. Creates directories for results, etc.
  task :setup do
    mkdir_p RBS_RESULTS_DIR, :verbose => $verbose
    mkdir_p WEB_DIR, :verbose => $verbose
  end

  task :run => :setup do
    puts "Running Ruby Benchmark Suite"
    puts "  Writing report to #{report_name}"

    Dir[RBS_DIR + "/**/bm_*.rb"].sort.each do |name|
      Dir.chdir File.dirname(name) do
        puts "  Running #{File.basename name}"
        system "#{command name}"
      end
    end

    puts "Done"
  end

  desc "Run all the benchmarks in DIR"
  task :dir => :setup do
    dir = ENV['DIR'] || raise("bench:dir needs DIR to be a directory")

    puts "Running all benchmarks in #{dir}"
    puts "  Writing report to #{report_name}"

    Dir[dir + "/**/bm_*.rb"].sort.each do |name|
      Dir.chdir File.dirname(name) do
        puts "  Running #{File.basename name}"
        system "#{command name}"
      end
    end

    puts "Done"
  end

  desc "Run only the benchmark specified by FILE"
  task :file => :setup do
    name = ENV['FILE'] || raise("bench:file needs FILE to be a filename")
    Dir.chdir File.dirname(name) do
      system "#{command name}"
    end
    puts "Writing report to #{report_name}"
  end
end
