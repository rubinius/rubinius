require 'types'

klasses = []

fd = File.open("auto.h", "w")

puts "#include \"shotgun.h\""

Rubinius::Types.each do |name, obj|
  prefix = "#{name}_"
  i = 0
  
  obj::TotalFields.each do |fel|
    fname = prefix.upcase + "f_" + fel.to_s.upcase
    fd.puts "#define #{fname} #{i}"
    i += 1
    fd.puts "#define #{prefix}get_#{fel}(obj) NTH_FIELD(obj, #{fname})"
    fd.puts "#define #{prefix}set_#{fel}(obj, val) SET_FIELD(obj, #{fname}, val)"
  end

  sz = obj::TotalFields.size  
  puts "OBJECT #{prefix}allocate_with_extra(STATE, int extra) {"
  puts "  return NEW_OBJECT(BASIC_CLASS(#{name}), #{sz} + extra);"
  puts "}"
  puts
  
  fd.puts "#define #{prefix}allocate(st) #{prefix}allocate_with_extra(st, 0)"
  fd.puts "#define #{prefix.upcase}FIELDS #{sz}"
  
  fd.puts "OBJECT #{prefix}allocate_with_extra(STATE, int extra);"
  
  klasses << [name, sz, obj::TotalFields.first == :instance_variables]
end

puts "\n/* Code to create the types into classes */\n\n"

klasses.each do |name, sz, has_ivars|
  fd.puts "OBJECT _#{name}_basic_class(STATE, OBJECT sup);"
  puts "OBJECT _#{name}_basic_class(STATE, OBJECT sup) {"
  puts "   OBJECT cls;"
  puts "   cls = class_allocate_with_extra(state, 0);"
  puts "   class_set_instance_fields(cls, I2N(#{sz}));"
  if has_ivars
    # HACK i hate that this is hardcoded here.
    flags = "0x02"
  else
    flags = "0"
  end
  puts "   class_set_instance_flags(cls, I2N(#{flags}));"
  puts "   class_set_superclass(cls, sup);"
  puts "   return cls;"
  puts "}"
  puts

  fd.puts "OBJECT _#{name}_class(STATE, OBJECT sup);"
  puts "OBJECT _#{name}_class(STATE, OBJECT sup) {"
  puts "   OBJECT cls;"
  puts "   cls = _#{name}_basic_class(state, sup);"
  puts "   object_create_metaclass(state, cls, (OBJECT)0);"
  puts "   return cls;"
  puts "}"
  puts
end
