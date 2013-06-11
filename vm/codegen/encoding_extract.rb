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

# This sort logic is critical for determining the encoding index, which is
# internally used in String#<=>.
# It must be IDENTICAL to MRI's counterpart logic.

def sort_by_encoding_index(files)
  files.sort_by do |file|
    token_pairs = file.scan(/(\D+)|(\d+)/)

    sort_key = token_pairs.map do |letters, digits|
      if letters
        letters
      else
        padded_numeric_sort_key(digits)
      end
    end

    sort_key.flatten
  end
end

def padded_numeric_sort_key(digits)
  [digits.length, digits.to_i]
end

encoding_files = []

File.open definitions, "wb" do |f|
  files = Dir["#{dir}/enc/*.[hc]"]
  sort_by_encoding_index(files).each do |name|
    parsed_lines = {
      :name => name,
      :definitions => [],
      :others => [],
    }

    readlines(name).each do |line|
      m = line.match /^ENC_([A-Z]+)\("([^"]+)"(,\s"?([^")]+)"?)?\);?/
      next unless m

      if m[1] == "DEFINE"
        next if ["ASCII", "UTF_8", "US_ASCII"].include?(m[4])

        parsed_lines[:definitions] << line
      else
        parsed_lines[:others] << line
      end
    end

    encoding_files << parsed_lines
  end

  f.puts "    // Encoding definitions"
  f.puts
  encoding_files.each do |encoding_file|
    definitions = encoding_file[:definitions]
    unless definitions.empty?
      f.puts "    // #{encoding_file[:name]}"
      definitions.each do |line|
        f.puts "    #{line}"
      end
      f.puts
    end
  end

  f.puts "    // Encoding aliases, dummies, replicates"
  f.puts
  encoding_files.each do |encoding_file|
    others = encoding_file[:others]
    unless others.empty?
      f.puts "    //#{encoding_file[:name]}"
      others.each do |line|
        f.puts "    #{line}"
      end
      f.puts
    end
  end
end
