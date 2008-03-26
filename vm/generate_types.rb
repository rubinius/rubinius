require "#{File.dirname(__FILE__)}/../lib/compiler/system_hints"

klasses = []

Compiler::Bootstrap::TYPES.each do |name, klass|
  prefix = "#{name}_"

  fields = Compiler::Bootstrap::HINTS[klass] || Hash.new
  sz = 0

  output = {}

  fields.each do |field_as_ivar, field_index|
    if field_as_ivar == :@__ivars__
      field_name = "instance_variables"
    elsif field_as_ivar == :@default
      field_name = "default_value"
    else
      field_name = field_as_ivar.to_s.gsub(/^@/,'')
    end

    output[field_index] = field_name;
    sz = field_index if field_index > sz
  end

  sz += 1 unless fields.empty?

  puts "namespace rubinius {"
  puts "  class #{klass.to_s.capitalize} : public BuiltinType {"
  puts "    public:"
  puts "    const static size_t fields = #{sz};"

  0.upto(sz - 1) do |idx|
    name = output[idx] || "field#{idx}"
    puts "    OBJECT #{name};"
  end

  puts "  };"
  puts "};"
  puts ""

  has_ivars = !fields[:@__ivars__].nil?
  klasses << [name, sz, has_ivars]
end
