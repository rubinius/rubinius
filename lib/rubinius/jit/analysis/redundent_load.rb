module Rubinius::JITCompiler
  class Analysis::RedundentLoad < Analysis::Pass
    def run_on_block(pc, block)
      registers = {}

      changed = false

      remove = []

      block.each do |i|
        case i
        when GetLocal
          reg, obj = registers.find do |k,v|
            v.kind_of?(GetLocal) and v.local == i.local
          end

          if obj
            i.replace Copy.new(i.destination, reg)
            changed = true
          else
            registers[i.destination] = i
          end
        when Store
          # If we're doing the same store, eliminate it
          if registers[i.destination] == i
            remove << i
          else
            registers[i.destination] = i
          end
        end
      end

      remove.each { |i| i.remove! }

      changed || !remove.empty?
    end
  end
end
