namespace :jit do
  task :generate_header do
    puts "GEN vm/llvm/types.cpp.gen"
    `vendor/llvm/Release/bin/llvm-as < vm/llvm/types.ll > vm/gen/types.bc`
    `vendor/llvm/Release/bin/llc -march=cpp -cppgen=contents -o vm/llvm/types.cpp.gen vm/gen/types.bc`
  end

  task :generate_offsets do
    classes = {}
    File.open "vm/llvm/types.ll" do |f|
      while line = f.gets
        if m1 = /%"?(struct|union)\.rubinius::([^"]*)"?\s*=\s*type\s*\{\n/.match(line)
          line = f.gets

          fields = []
          while line.strip != "}"
            if m2 = /;\s*(.*)/.match(line)
              fields << m2[1].strip
            else
              fields << nil
            end

            line = f.gets
          end

          classes[m1[2]] = fields
        end
      end
    end

    File.open "vm/llvm/offset_specific.hpp", "w" do |f|
      f.puts "namespace offset {"

      classes.each do |name, fields|
        f.puts "namespace #{name.gsub('::', '_')} {"
        fields.each_with_index do |n, idx|
          if n
            f.puts "  const static int #{n} = #{idx};"
          end
        end
        f.puts "}"
      end

      f.puts "}"
    end
  end
end
