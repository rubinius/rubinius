dir, definitions = ARGV

if defined? Encoding
  def readlines(name)
    IO.readlines(name, :encoding => "ascii-8bit")
  end
else
  def readlines(name)
    IO.readlines(name)
  end
end

File.open definitions, "wb" do |f|
  Dir["#{dir}/enc/*.[hc]"].sort.each do |name|
    f.puts "    // #{name}"

    readlines(name).each do |line|
      m = line.match /^ENC_([A-Z]+)\("([^"]+)"(,\s"?([^")]+)"?)?\);?/
      next unless m

      unless m[1] == "DEFINE" and ["ASCII", "UTF_8", "US_ASCII"].include?(m[4])
        f.puts "    #{line}"
      end
    end

    f.puts
  end
end
