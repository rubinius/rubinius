class Compiler
  class TextGenerator
    def initialize
      @text = ""
      @label = 0
    end
    
    attr_reader :text
    
    class Label
      def initialize(gen, idx)
        @gen = gen
        @index = idx
      end
      
      def set!
        @gen.set_label(@index)
      end
    end
    
    def new_label
      Label.new(self, @label += 1)
    end
    
    def set_label(idx)
      @text << "l#{idx}:\n"
    end
    
    def run(node)
      node.bytecode(self)
    end
    
    def set_line(line, file)
      @text << "#line #{line}\n"
    end
    
    def close
    end
    
    def method_missing(op, *args)
      if args.empty?
        @text << "#{op}\n"
      else
        @text << "#{op} #{args.join(' ', :inspect)}\n"
      end
    end
  end
end