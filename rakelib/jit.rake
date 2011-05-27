namespace :jit do
  task :regenerate_header do
    puts "GEN vm/llvm/types.ll, vm/llvm/types.cpp.gen"

    classes = %w!
                 rubinius::ObjectFlags
                 rubinius::HeaderWord
                 rubinius::ObjectHeader
                 rubinius::Object
                 rubinius::StackVariables
                 rubinius::CallFrame
                 rubinius::UnwindInfo
                 rubinius::VariableScope
                 rubinius::CompiledMethod
                 rubinius::Executable
                 rubinius::Dispatch
                 rubinius::Arguments
                 rubinius::Tuple
                 rubinius::Array
                 rubinius::Class
                 rubinius::Module
                 rubinius::StaticScope
                 rubinius::InstructionSequence
                 rubinius::InlineCache
                 rubinius::InlineCacheHit
                 rubinius::BlockEnvironment
                 rubinius::BlockInvocation
                 rubinius::Numeric
                 rubinius::Float
                 rubinius::jit::RuntimeData
                 rubinius::CallUnit
                 rubinius::MethodCacheEntry
                 memory::Address
                 jit_state!
    require 'tempfile'

    files = %w!vm/call_frame.hpp
               vm/arguments.hpp
               vm/dispatch.hpp
               vm/inline_cache.hpp
               vm/builtin/block_environment.hpp
               vm/builtin/integer.hpp
               vm/builtin/float.hpp
               vm/llvm/jit_runtime.hpp!
    path = "llvm-type-temp.cpp"

    File.open(path, "w+") do |f|
      files.each do |file|
        f.puts "#include \"#{file}\""
      end

      i = 0
      classes.each do |klass|
        f.puts "void useme#{i}(#{klass}* thing);"
        f.puts "void blah#{i}(#{klass}* thing) { useme#{i}(thing); }"
        i += 1
      end
    end

    str = `llvm-g++ -I. -Ivm -Ivm/external_libs/libtommath -emit-llvm -S -o - "#{path}"`

    return unless $?.exitstatus == 0

    File.unlink path

    types = []

    str.split("\n").each do |line|
      classes.each do |klass|
        if /%"?(struct|union).#{klass}(::\$[^\s]+)?"? = type/.match(line)
          types << line
        end
      end
    end

    opaque = %w!VM TypeInfo VMMethod Fixnum Symbol Selector LookupTable MethodTable
                jit::RuntimeDataHolder Inliners!

    File.open("vm/gen/types.ll","w+") do |f|
      opaque.each do |o|
        f.puts "%\"struct.rubinius::#{o}\" = type opaque"
      end
      f.puts(*types)
    end

    `vm/external_libs/llvm/Release/bin/llvm-as < vm/gen/types.ll > vm/gen/types.bc`
    `vm/external_libs/llvm/Release/bin/llc -march=cpp -cppgen=contents -o vm/llvm/types.cpp.gen vm/gen/types.bc`
  end

  task :generate_header do
    puts "GEN vm/llvm/types.cpp.gen"
    `vm/external_libs/llvm/Release/bin/llvm-as < vm/llvm/types.ll > vm/gen/types.bc`
    `vm/external_libs/llvm/Release/bin/llc -march=cpp -cppgen=contents -o vm/llvm/types.cpp.gen vm/gen/types.bc`
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
