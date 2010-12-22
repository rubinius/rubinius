require 'rubygems'
require 'hpricot'

class Profiler

  class Leaf
    def initialize(method, total)
      @method = method
      @total = total
    end

    attr_reader :method, :total
  end

  class Method
    def initialize(id)
      @id = id
      @leaves = []
    end

    attr_accessor :file, :line, :name, :total, :called

    def add_leaf(method, total)
      @leaves << Leaf.new(method, total)
    end

    def total_time
      @total.to_f / 1_000_000_000
    end

    def self_time
      time = @leaves.inject(@total) { |acc, leaf| acc - leaf.total }
      time.to_f / 1_000_000_000
    end

    def inspect
      "#<Profiler::Method #{name}>"
    end

  end

  def initialize
    @methods = Hash.new do |h,k|
      h[k] = Method.new(k)
    end
  end

  attr_reader :methods

  def load(data)
    doc = Hpricot(data)

    top = (doc / :profile)
    (top / :method).each do |m|
      meth = @methods[m[:id].to_i]
      meth.file = m[:file]
      meth.line = m[:line].to_i
      meth.name = m[:name]
      meth.total = m[:total].to_i
      meth.called = m[:called].to_i

      (m / :leaf).each do |l|
        meth.add_leaf @methods[l[:id].to_i], l[:total].to_i
      end
    end
  end

  def biggest
    sorted = @methods.sort do |a,b|
      b[1].total <=> a[1].total
    end

    sorted[0][1]
  end
end

prof = Profiler.new
prof.load File.read(ARGV.shift)
meth = prof.biggest

puts "#{meth.name}  #{meth.total_time} #{meth.self_time}"
