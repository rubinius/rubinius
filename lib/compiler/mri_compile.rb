require File.dirname(__FILE__) + '/mri_shim'

def decode_cm(cm)
  puts "= #{cm.name} (0x#{cm.object_id.to_s(16)}) ======================"
  puts "= stack:#{cm.stack_size}, locals:#{cm.local_count}"
  puts "= #{cm.describe}"
  puts cm.decode

  extra = cm.literals.to_a.find_all { |l| l.kind_of? Rubinius::CompiledMethod }

  until extra.empty?
    sub = extra.shift
    decode_cm(sub);
    extra += sub.literals.to_a.find_all { |l| l.kind_of? Rubinius::CompiledMethod }
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
    if l1.kind_of? Rubinius::CompiledMethod
      compare_cm(l1, l2)
    else
      if l1 != l2
        puts "literal #{i} differs: #{l1.inspect} != #{l2.inspect} (delta #{(l1 - l2).abs})"
      end
    end
  end
end

class Float
  undef_method :==
  def ==(other)
    return false unless Numeric === other

    # Simple case
    return true if self.eql? other

    # This cannot be a simple abs magnitude test
    # because the magnitude of the tolerance depends
    # on the sign of the exponent. Instead do a
    # magnitude test on the normalized fraction. A
    # further complication occurs when the exponents
    # differ by 1.
    a = Math.frexp self
    b = Math.frexp other
    e = (a.last - b.last).abs
    if e == 0
      return (a.first - b.first).abs < 5e-14
    elsif e == 1
      return (a.first - b.first).abs < 0.5
    else
      return false
    end
  end
end

# Methods provided by Rubinius. We're treating an MRI String
# like a Rubinius ByteArray in compiled_file.rb
class String
  def self.from_bytearray(data, start, count)
    data[start, count]
  end

  # In Rubinius, this is on ByteArray
  def locate(pattern, start)
    count = index pattern, start
    count ? count + pattern.size : count
  end

  def data
    self
  end
end

def mri_compile file, output = nil, decode = false, flags = []
  puts "Compiling #{file}"

  output ||= "#{file}c"
  top = Compiler.compile_file(file, flags)
  mar = Rubinius::CompiledFile::Marshal.new

  decode_cm(top) if decode

  if output
    str = mar.marshal(top)
    nt = mar.unmarshal(str)
    unless top == nt
      puts "FAILED ROUND TRIP."
      compare_cm(top, nt)
      raise SyntaxError, "compiler borked on #{file}"
    end

    Rubinius::CompiledFile.dump top, output
  end
end

if __FILE__ == $0 then
  flags = []
  decode = false
  output = nil

  while arg = ARGV.shift
    case arg
    when "-d" then
      decode = true
      $VERBOSE = true
    when /-f(.*)/ then
      flags << $1
    else
      ARGV.unshift arg
      break
    end
  end

  file   = ARGV.shift
  output = ARGV.shift

  begin
    mri_compile file, output, decode, flags
  rescue SyntaxError
    exit 1
  end
end
