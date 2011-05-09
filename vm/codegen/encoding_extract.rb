dir, definitions = ARGV

File.open definitions, "wb" do |f|
  f.puts <<-EOD
    // Encodings
  EOD
end
