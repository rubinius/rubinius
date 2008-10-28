require 'benchmark'

total = (ENV['TOTAL'] || 10_000).to_i

def make_character
  97 + rand(26)
end

def make_string
  (0..4).inject("") { |s,i| s << make_character }
end

def make_symbol
  make_string.to_sym
end

LookupTable = Hash unless defined?(LookupTable)
table = LookupTable.new
hash = {}

50.times do
  sym = make_symbol
  str = make_string
  table[sym] = str
  hash[sym] = str
end
keys = table.keys
strings = table.values

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { |i| keys.each {} }
  end

  x.report("LookupTable#[]") do
    total.times do
      keys.each { |k| table[k] }
    end
  end

  x.report("Hash#[]") do
    total.times do
      keys.each { |k| hash[k] }
    end
  end

  x.report("LookupTable#[]=") do
    t = LookupTable.new
    total.times do
      keys.size.times do |i|
        t[keys[i]] = strings[i]
      end
    end
  end

  x.report("Hash#[]=") do
    h = {}
    total.times do
      keys.size.times do |i|
        h[keys[i]] = strings[i]
      end
    end
  end

  x.report("LookupTable#each") do
    total.times do
      table.each { |k,v| k; v }
    end
  end

  x.report("Hash#each") do
    total.times do
      hash.each { |k,v| k; v }
    end
  end

  x.report("LookupTable#keys") do
    total.times { table.keys }
  end

  x.report("Hash#keys") do
    total.times { hash.keys }
  end
end
