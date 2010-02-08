klass = eval("#{ARGV.shift}")

klass_name = klass.to_s.downcase

map = {
  "%" => "modulo_op",
  "*" => "times_op",
  "+" => "plus_op",
  "<<" => "append_op",
  "<=>" => "compare_op",
  "==" => "equal_op",
  "=~" => "regex_equal_op",
  "[]" => "aref_op",
  "[]=" => "aset_op"
}

unless File.directory?("methods/#{klass_name}")
  Dir.mkdir("methods/#{klass_name}")
end

klass.instance_methods(false).each do |meth|
  if sub = map[meth]
    meth = sub
  end

  meth.sub!(/\!$/, "_bang")
  meth.sub!(/\?$/, "_eh")

  file = "methods/#{klass_name}/#{meth}.rb"
  unless File.exists?(file)
    puts file

    File.open(file, "w") do |f|
      f << <<-CODE
def Bench.run
  i = 0
  while @should_run
    # #{klass_name}##{meth}(...)
    raise "#{klass_name}##{meth}(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end
      CODE
    end
  end
end
