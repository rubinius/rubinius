# encoding: utf-8

string = ""

reader = Encoding::Converter.new(Encoding::UTF_8, Encoding::UTF_8_MAC)
source = string.dup
out = "A\u0302".force_encoding(Encoding::UTF_8_MAC)

loop do
  puts "Source: #{source.inspect} l: #{source.length} bs: #{source.bytesize}"
  reader.primitive_convert(source, out)
  _, _, _, error_bytes, read_again_bytes = reader.primitive_errinfo
  if error_bytes.nil?
    puts "Error bytes: nil"
  else
    puts "Error bytes: #{error_bytes.inspect} l: #{error_bytes.length} bs: #{error_bytes.bytesize}"
  end
  #puts "Read again bytes: #{read_again_bytes.inspect} l: #{read_again_bytes.length} bs: #{read_again_bytes.bytesize}"
  break if error_bytes.nil?
  puts "Out: #{out.inspect} enc: #{out.encoding} l: #{out.length} bs: #{out.bytesize}"
  out << error_bytes.encode(Encoding::UTF_8_MAC, Encoding::Windows_1252, invalid: :replace, undef: :replace)
  puts "Out: #{out.inspect} enc: #{out.encoding} l: #{out.length} bs: #{out.bytesize}"
  puts
end

reader.finish

out.encode!(Encoding::UTF_8)

puts "Out: #{out.inspect} enc: #{out.encoding} l: #{out.length} bs: #{out.bytesize}"
