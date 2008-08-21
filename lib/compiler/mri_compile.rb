require 'stringio'

require File.dirname(__FILE__) + '/mri_shim'

flags = []
decode = false
output = nil

while arg = ARGV.shift
  if arg == "-d"
    decode = true
    $VERBOSE = true
  elsif /-f(.*)/.match(arg)
    flags << $1
  else
    ARGV.unshift arg
    break
  end
end

file = ARGV.shift
output = ARGV.shift
puts "Compiling #{file}"

top = Compiler.compile_file(file, flags)

mar = Compiler::Marshal.new

def decode_cm(cm)
  puts "= #{cm.name} (0x#{cm.object_id.to_s(16)}) ======================"
  puts "= stack:#{cm.stack_size}, locals:#{cm.local_count}"
  puts "= #{cm.describe}"
  puts cm.decode

  extra = cm.literals.to_a.find_all { |l| l.kind_of? CompiledMethod }

  until extra.empty?
    sub = extra.shift
    decode_cm(sub);
    extra += sub.literals.to_a.find_all { |l| l.kind_of? CompiledMethod }
  end
end

def compare_cm(a,b)
  m = [:primitive, :name, :iseq, :stack_size, :local_count,
       :required_args, :total_args, :splat, :literals, 
       :exceptions, :lines, :file, :local_names]

  m.each do |n|
    if (l1 = a.__send__(n)) != (l2 = b.__send__(n))
      puts "#{n} differs: #{l1.inspect} != #{l2.inspect}"
    end
  end

  0.upto(a.literals.size - 1) do |i|
    l1 = a.literals[i]
    l2 = b.literals[i]
    if l1.kind_of? CompiledMethod
      compare_cm(l1, l2)
    else
      if l1 != l2
        puts "literal #{i} differs: #{l1.inspect} != #{l2.inspect}"
      end
    end
  end
end

class Float
  undef_method :==
  def ==(other)
    (self - other).abs < 1e-14
  end
end

decode_cm(top) if decode

if output
  str = mar.marshal(top)
  nt = mar.unmarshal(str)
  unless top == nt
    puts "FAILED ROUND TRIP."
    compare_cm(top, nt)
    exit 1
  end

  File.open(output, "w") do |f|
    cf = Compiler::CompiledFile.new "!RBIX", 1, "x"
    cf.encode_to f, top
  end

end
