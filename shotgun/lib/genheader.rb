require '../../compiler/bytecode/system_hints'

klasses = []

fd = File.open("auto.h", "w")

puts "#include \"shotgun.h\""

Rubinius::Bootstrap::TYPES.each do |name, klass|
  prefix = "#{name}_"
  
  fields = Rubinius::Bootstrap::HINTS[klass] || Hash.new
  fields.each do |field_as_ivar, field_index|
    if field_as_ivar == :@__ivars__
      field_name = "instance_variables" 
    else
      field_name = field_as_ivar.to_s.gsub(/^@/,'') 
    end
    func_name = "#{prefix.upcase}f_#{field_name.upcase}"
    fd.puts "#define #{func_name} #{field_index}"
    fd.puts "#define #{prefix}get_#{field_name}(obj) NTH_FIELD(obj, #{func_name})"
    fd.puts "#define #{prefix}set_#{field_name}(obj, val) SET_FIELD(obj, #{func_name}, val)"
  end

  sz = fields.size
  puts "OBJECT #{prefix}allocate_with_extra(STATE, int extra) {"
  puts "  return NEW_OBJECT(BASIC_CLASS(#{name}), #{sz} + extra);"
  puts "}"
  puts
  puts "OBJECT #{prefix}allocate_mature(STATE, int extra) {"
  puts "  return object_memory_new_object_mature(state->om, BASIC_CLASS(#{name}), #{sz} + extra);"
  puts "}"
  puts
  
  
  fd.puts "#define #{prefix}allocate(st) #{prefix}allocate_with_extra(st, 0)"
  fd.puts "#define #{prefix.upcase}FIELDS #{sz}"
  
  fd.puts "OBJECT #{prefix}allocate_with_extra(STATE, int extra);"
  fd.puts "OBJECT #{prefix}allocate_mature(STATE, int extra);"
  
  has_ivars = !fields[:@__ivars__].nil?
  klasses << [name, sz, has_ivars]
end

puts "\n/* Code to create the types into classes */\n\n"

klasses.each do |name, sz, has_ivars|
  fd.puts "OBJECT _#{name}_basic_class(STATE, OBJECT sup);"
  puts "OBJECT _#{name}_basic_class(STATE, OBJECT sup) {"
  puts "   OBJECT cls;"
  puts "   cls = class_allocate_mature(state, 0);"
  puts "   class_set_instance_fields(cls, I2N(#{sz}));"
  puts "   class_set_has_ivars(cls, #{has_ivars ? 'Qtrue' : 'Qfalse'});"
  puts "   class_set_needs_cleanup(cls, Qfalse);"
  puts "   class_set_object_type(cls, I2N(ObjectType));"
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
