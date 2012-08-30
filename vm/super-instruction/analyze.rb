require 'pp'

if defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'
  Object.const_set(:Compiler, Compile.compiler)
  require 'compiler/text'
else
  $: << 'lib'
  require File.join(File.dirname(__FILE__), '..', 'compiler', 'mri_shim')
end

def record_block(data, block)
  record_seq data, block.dup
  0.upto(block.size - 3) do |start|
    2.upto(10) do |size|
      seq = block[start, size]
      record_seq data, seq if seq.size > 1
    end
  end
end

def record_seq(data, seq)
  count = data[seq]
  if count
    data[seq] = count + 1
  else
    data[seq] = 1
  end
end

Terms = [:goto, :goto_if_false, :goto_if_true]

def walk_stream(stream, data)
  seq = []
  stream.each do |inst|
    seq << inst.first
    if Terms.include? inst.first
      if seq.size > 1
        record_block data, seq
      end
      seq = []
    end
  end

  record_block data, seq if seq.size > 1
end

def update_data(stream, data, extra)
  stream.each_with_index do |inst, i|
    combo = [inst.first]
    extra.times do |x|
      next_inst = stream[i + x + 1]
      return unless next_inst
      combo << next_inst.first
    end
    count = data[combo]
    if count
      data[combo] = count + 1
    else
      data[combo] = 1
    end
  end
end

def describe_compiled_code(code, data, max)
  extra = code.literals.to_a.find_all { |l| l.kind_of? CompiledCode }

  name = code.name ? code.name.inspect : 'anonymous'
  stream = code.iseq.decode

  walk_stream stream, data
=begin
  2.upto(max) do |size|
    update_data stream, data[size], size - 1
  end
=end

  until extra.empty?
    sub = extra.shift
    describe_compiled_code(sub, data, max)
    extra += sub.literals.to_a.find_all { |l| l.kind_of? CompiledCode }
  end

end

# Temporary workaround for Rubinius bug in __FILE__ paths
if __FILE__.include?($0) then
  flags = []
  file = nil

  while arg = ARGV.shift
    case arg
    when /-I(.+)/ then
      other_paths = $1[2..-1].split(":")
      other_paths.each { |n| $:.unshift n }
    when /-f(.+)/ then
      flags << $1
    else
      file = arg
      break
    end
  end

  unless file
    interactive()
    exit 0
  end

  require 'yaml'

  out = ARGV.shift or "analyze.yml"

  max = 10

  puts "Gathering data on #{file}..."

  if File.exists?(out)
    data = Marshal.load File.read(out)
  else
    data = Hash.new
=begin
    2.upto(max) do |size|
      data[size] = Hash.new
    end
=end
  end

  begin
    top = Compiler.compile_file(file, flags)
    describe_compiled_code(top, data, max)

  rescue SyntaxError
    exit 1
  end

  File.open out, "w" do |f|
    f << Marshal.dump(data)
  end
end
