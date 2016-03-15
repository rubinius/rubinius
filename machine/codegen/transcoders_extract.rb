dir, definitions = ARGV

if defined? Encoding
  def read(name)
    IO.read(name, :encoding => "ascii-8bit")
  end
else
  def read(name)
    IO.read(name)
  end
end

File.open definitions, "wb" do |f|
  re = /^static\s*const\s*rb_transcoder\s*\n
                      rb_\w+\s*=\s*\{\s*\n
                      \s*"([^"]+)",\s*"([^"]+)"/mx

  Dir["#{dir}/*.c"].sort.each do |name|
    f.puts "    // #{name}"

    lib = File.basename name, ".c"

    read(name).scan(re) do |from, to|
      f.puts %[    TRANS_DECLARE("#{from}", "#{to}", "#{lib}");]
    end

    f.puts
  end
end
