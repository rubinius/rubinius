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
RESULTS_BASEDIR = BASEDIR + "/benchmark/results"
WEB_DIR         = RESULTS_BASEDIR + "/web"

ITERATIONS      = (ENV['ITERATIONS'] || 5).to_i
TIMEOUT         = (ENV['TIMEOUT'] || 300).to_i
VM              = ENV['VM'] || "#{BASEDIR}/bin/rbx"
ENV['VM']       = VM

BASELINE        = ENV['BASELINE'] ? File.expand_path(ENV['BASELINE']) : nil
BASELINE_ID     = ENV['BASELINE_ID'] || "no baseline id"
BASELINE_MIN    = (ENV['BASELINE_MIN'] || 0).to_i
BASELINE_MAX    = (ENV['BASELINE_MAX'] || 7).to_i
GROUP_NAME      = ENV['GROUP'] || File.basename(VM.split.first)
RESULTS_DIR     = ENV['RESULTS'] || RESULTS_BASEDIR
TEMPLATE        = ENV['TEMPLATE'] || RESULTS_BASEDIR + "/templates/basic.erb"
HTMLOUT         = ENV['HTMLOUT'] || WEB_DIR + "/index.html"


def command(name)
  "ruby #{MONITOR} #{TIMEOUT} '#{VM}' #{RUNNER} #{name} #{ITERATIONS} #{report}"
end

# Cache the name so it is only generated once during an invocation.
# Eliminates having to save the name and pass it around.
def report
  @report ||= "#{RESULTS_DIR}/#{GROUP_NAME}-#{Time.now.strftime "%d-%m-%Y-%H%M"}.yaml"
end

def report_name
  report[(BASEDIR.size+1)..-1]
end

def field_name
  field = ENV['FIELD'] || "min"
  unless ["max", "min", "median", "mean"].include?(field)
    raise "FIELD must be one of max, min, median, mean"
  end
  field
end

def parse_date(text)
  if /-(\d+)-(\d+)-(\d+)-(\d+).yaml/ =~ text
    Time.local($3, $2, $1).to_i * 1000
  else
    t = Time.now
    Time.local(t.year, t.mon, t.day).to_i * 1000
  end
end

def group_id(name)
  /([^\d]+)-\d+/ =~ File.basename(name, ".*").split("/").last
  $1
end

class Graph
  class Point
    attr_accessor :x, :y, :baseline

    def initialize(x, y=0.0)
      @x = x
      @y = y
      @valid = true
      @baseline = 0.0
    end

    def valid?
      @valid
    end

    def invalid
      @valid = false
    end

    def data(delta=false)
      return "null" unless valid?
      (delta ? [x, @baseline.to_f / y] : [x, y]).inspect
    end
  end

  class Line
    attr_reader :label

    def initialize(label)
      /bm_([^.]*)\.rb-(\d+)$/ =~ label
      input = $2
      name = $1.gsub(/_/, " ")
      @label  = "#{name} (#{input})"
      @points = Hash.new { |h,k| h[k] = Point.new(k) }
    end

    def set_point(x, y)
      @points[x].y = y
    end

    def set_baseline(baseline)
      @points.each_value { |p| p.baseline = baseline }
    end

    def invalid(date)
      @points[date].invalid
    end

    def data(delta=false)
      points = @points.values.sort { |a, b| a.x <=> b.x }
      str = points.map { |p| p.data(delta) }.join(",")
      %[{ label: "#{label}", data: [#{str}] }]
    end
  end

  class LineSet
    attr_reader :lines

    def initialize(set_name)
      @set_name = set_name
      @lines    = Hash.new { |h,k| h[k] = Line.new(k) }
    end

    def invalid(date)
      @lines.each_value { |line| line.invalid date }
    end

    def data(delta=false)
      @lines.values.map { |line| line.data(delta) }.join(", ")
    end
  end

  attr_reader :id, :title, :linesets

  def initialize(id)
    @id       = id.gsub(/\W/, "_")
    @title    = id.gsub(/\W/, " ")
    @linesets = Hash.new { |h,k| h[k] = LineSet.new(k) }
  end

  def data(delta=false)
    @linesets.values.map { |lineset| lineset.data(delta) }.join(", ")
  end
end

class GraphEnvironment
  attr_accessor :graphs, :field, :min_date, :max_date, :width, :height,
                :baseline_id, :baseline_min, :baseline_max

  def initialize
    @min_date = Time.now.to_i * 1000
    @max_date = 0
    @width = 800
    @height = 400
  end

  def get_binding
    binding
  end
end

desc "Run the RBS benchmarks"
task :bench => 'bench:run'

namespace :bench do
  desc "Plot the RBS benchmark results"
  task :results => :setup do
    require 'yaml'

    env = GraphEnvironment.new
    env.field  = field_name
    env.graphs = Hash.new { |h,k| h[k] = Graph.new(k) }

    Dir[RESULTS_DIR + "/**/*.yaml"].sort.each do |name|
      next if BASELINE == name

      graph = env.graphs[group_id(name)]
      date  = parse_date name

      env.min_date = date if date < env.min_date
      env.max_date = date if date > env.max_date

      File.open name, "r" do |file|
        YAML.load_documents file do |doc|
          lineset = graph.linesets[doc["name"]]

          if doc.key? "status" and doc["status"] != "success"
            lineset.invalid date
          end

          next unless doc.key? env.field

          line = lineset.lines["#{doc["name"]}-#{doc["parameter"]}"]
          line.set_point date, doc[env.field]
        end
      end
    end

    if BASELINE
      env.baseline_id  = BASELINE_ID
      env.baseline_min = BASELINE_MIN
      env.baseline_max = BASELINE_MAX

      File.open BASELINE, "r" do |file|
        YAML.load_documents file do |doc|
          next unless doc.key? env.field

          env.graphs.each_value do |graph|
            lineset = graph.linesets[doc["name"]]
            line = lineset.lines["#{doc["name"]}-#{doc["parameter"]}"]
            line.set_baseline doc[env.field]
          end
        end
      end
    end

    require 'erb'
    rhtml = ERB.new(IO.read(TEMPLATE))

    File.open HTMLOUT, "w" do |f|
      f.puts rhtml.result(env.get_binding)
    end
  end

  desc "Generate a CSV file of results"
  task :to_csv => :setup do
    require 'yaml'

    field  = field_name
    header = ["Benchmark file", "input"]
    data   = Hash.new { |h,k| h[k] = {} }
    status = Hash.new { |h,k| h[k] = {} }

    Dir[RESULTS_DIR + "/**/*.yaml"].sort.each do |name|
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

    csv_report = "#{RESULTS_DIR}/report-#{Time.now.strftime "%d-%m-%Y-%H%M"}.csv"
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
    mkdir_p RESULTS_DIR, :verbose => $verbose
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
