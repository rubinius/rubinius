namespace :jit do
  task :generate_header do
    puts "GEN vm/llvm/types{32|64}.cpp.gen"
    `llvm-as < vm/llvm/types32.ll > vm/gen/types32.bc`
    `llvm-as < vm/llvm/types64.ll > vm/gen/types64.bc`
    `llc -march=cpp -cppgen=contents -o vm/llvm/types32.cpp.gen vm/gen/types32.bc`
    `llc -march=cpp -cppgen=contents -o vm/llvm/types64.cpp.gen vm/gen/types64.bc`
  end

  task :generate_offsets do
    classes = {}
    File.open "vm/llvm/types64.ll" do |f|
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

    File.open "vm/llvm/offset.hpp", "w" do |f|
      f.puts "#ifndef RBX_LLVM_OFFSET_HPP"
      f.puts "#define RBX_LLVM_OFFSET_HPP"
      f.puts ""
      f.puts "namespace offset {"

      classes.sort.each do |name, fields|
        f.puts "namespace #{name.gsub('::', '_')} {"
        fields.each_with_index do |n, idx|
          if n
            f.puts "  const static int #{n} = #{idx};"
          end
        end
        f.puts "}"
      end

      f.puts "}"
      f.puts "#endif"
    end
  end
end
