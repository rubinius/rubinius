module Rubinius

  class CompileError < RuntimeError
  end

  class Compiler
    attr_accessor :parser, :generator, :encoder, :packager, :writer

    def self.compiler_error(msg, orig)
      if defined?(RUBY_ENGINE) and RUBY_ENGINE == "rbx"
        raise Rubinius::CompileError, msg, orig
      else
        orig.message.replace("#{orig.message} - #{msg}")
        raise orig
      end
    end

    def self.compiled_name(file)
      if file.suffix? ".rb"
        file + "c"
      else
        file + ".compiled.rbc"
      end
    end

    def self.compile(file, output=nil, line=1, transforms=:default)
      compiler = new :file, :compiled_file

      parser = compiler.parser
      parser.root AST::Script

      if transforms.kind_of? Array
        transforms.each { |t| parser.enable_category t }
      else
        parser.enable_category transforms
      end

      parser.input file, line

      writer = compiler.writer
      writer.name = output ? output : compiled_name(file)

      begin
        compiler.run
      rescue SyntaxError => e
        raise e
      rescue Exception => e
        compiler_error "Error trying to compile #{file}", e
      end

    end

    # Match old compiler's signature
    def self.compile_file_old(file, flags=nil)
      compile_file file, 1
    end

    def self.compile_file(file, line=1)
      compiler = new :file, :compiled_method

      parser = compiler.parser
      parser.root AST::Script
      parser.default_transforms
      parser.input file, line

      begin
        compiler.run
      rescue Exception => e
        compiler_error "Error trying to compile #{file}", e
      end
    end

    def self.compile_string(string, file="(eval)", line=1)
      compiler = new :string, :compiled_method

      parser = compiler.parser
      parser.root AST::Script
      parser.default_transforms
      parser.input string, file, line

      compiler.run
    end

    class LRUCache
      class Entry
        attr_reader :hits, :key
        attr_accessor :value, :next_entry, :prev_entry

        def initialize(key, value)
          @key = key
          @value = value
          @hits = 0
          @next_entry = nil
          @prev_entry = nil
        end

        def insert_after(entry)
          nxt = entry.next_entry

          @prev_entry = entry
          @next_entry = nxt

          entry.next_entry = self
          nxt.prev_entry = self if nxt
        end

        def detach!
          @next_entry.prev_entry = @prev_entry if @next_entry
          @prev_entry.next_entry = @next_entry if @prev_entry

          @next_entry = nil
          @prev_entry = nil
        end

        def become_first!
          @prev_entry = nil
        end

        def inc!
          @hits += 1
        end
      end

      def initialize(total)
        @cache = {}
        @total = total
        @current = 0

        @last_entry = nil
        @first_entry = nil
        @misses = 0
      end

      attr_reader :current, :misses

      def clear!
        @cache = {}
        @current = 0
        @last_entry = nil
        @first_entry = nil
      end

      def explain
        entry = @first_entry
        while entry
          str, layout = entry.key
          puts "hits: #{entry.hits}"
          puts "layout: #{layout.inspect}"
          puts "<STRING>"
          puts str
          puts "</STRING>"

          entry = entry.next_entry
        end
      end

      def retrieve(key)
        if entry = @cache[key]
          entry.inc!

          unless entry == @last_entry
            entry.detach!
            entry.insert_after @last_entry
            @last_entry = entry
          end

          return entry.value
        end

        @misses += 1

        nil
      end

      def set(key, value)
        if entry = @cache[key]
          entry.value = value

          unless entry == @last_entry
            entry.insert_after @last_entry
            @last_entry = entry
          end

          return value
        end

        if @current >= @total
          entry = @first_entry
          if @first_entry = entry.next_entry
            @first_entry.become_first!
          end

          @cache.delete entry.key
        else
          @current += 1
        end

        entry = Entry.new(key, value)

        if @last_entry
          entry.insert_after @last_entry
        else
          @first_entry = entry
        end

        @last_entry = entry

        @cache[key] = entry
      end
    end

    total = Rubinius::Config['eval.cache']

    case total
    when Fixnum
      if total == 0
        @eval_cache = nil
      else
        @eval_cache = LRUCache.new(total)
      end
    when false
      @eval_cache = nil
    else
      @eval_cache = LRUCache.new(50)
    end

    def self.eval_cache
      @eval_cache
    end

    def self.compile_eval(string, variable_scope, file="(eval)", line=1)
      if ec = @eval_cache
        layout = variable_scope.local_layout
        if cm = ec.retrieve([string, layout])
          return cm
        end
      end

      compiler = new :string, :compiled_method

      parser = compiler.parser
      parser.root AST::EvalExpression
      parser.default_transforms
      parser.input string, file, line

      compiler.generator.variable_scope = variable_scope

      cm = compiler.run

      if ec
        ec.set([string.dup, layout], cm)
      end
      return cm
    end

    def self.compile_test_bytecode(string, transforms)
      compiler = new :string, :bytecode

      parser = compiler.parser
      parser.root AST::Snippet
      parser.input string
      transforms.each { |x| parser.enable_transform x }

      compiler.generator.processor TestGenerator

      compiler.run
    end

    def initialize(from, to)
      @start = Stages[from].new self, to
    end

    def run
      @start.run
    end
  end
end
