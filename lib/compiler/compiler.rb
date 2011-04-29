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

    if @object_db = Rubinius::Config['rbc.db']
      @object_db = ".rbx" unless @object_db.kind_of? String
    end

    def self.compiled_name(file)
      if file.suffix? ".rb"
        path = file + "c"
      else
        path = file + ".compiled.rbc"
      end

      if db = @object_db and !File.exists?(path)
        full = File.expand_path(file)
        hash = Rubinius.invoke_primitive :sha1_hash, full
        dir = hash[0,2]

        path = "#{db}/#{dir}/#{hash}"
      end

      path
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

        def insert_before(entry)
          prev = entry.prev_entry

          @prev_entry = prev
          @next_entry = entry

          entry.prev_entry = self
          prev.next_entry = self if prev
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

        @head = Entry.new(nil, nil)
        @tail = Entry.new(nil, nil)

        @tail.insert_after(@head)

        @misses = 0
        @lock = Rubinius::Channel.new
        @lock << nil # prime
      end

      attr_reader :current, :misses

      def clear!
        @lock.as_lock do
          @cache = {}
          @current = 0

          @head = Entry.new(nil, nil, -1)
          @tail = Entry.new(nil, nil, -2)

          @tail.insert_after(@head)
        end
      end

      def explain
        entry = @head.next_entry
        while entry != @tail
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
        @lock.as_lock do
          if entry = @cache[key]
            entry.inc!

            entry.detach!
            entry.insert_before @tail

            return entry.value
          end

          @misses += 1

          nil
        end
      end

      def set(key, value)
        @lock.as_lock do
          if entry = @cache[key]
            entry.value = value

            entry.detach!
            entry.insert_before @tail

            return value
          end

          if @current == @total
            entry = @head.next_entry

            entry.detach!

            @cache.delete entry.key
          else
            @current += 1
          end

          entry = Entry.new(key, value)

          entry.insert_before @tail

          @cache[key] = entry
        end
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

      compiler = new :eval, :compiled_method

      parser = compiler.parser
      parser.root AST::EvalExpression
      parser.default_transforms
      parser.input string, file, line

      compiler.generator.variable_scope = variable_scope

      cm = compiler.run

      cm.add_metadata :for_eval, true

      if ec and parser.should_cache?
        ec.set([string.dup, layout], cm)
      end

      return cm
    end

    def self.construct_block(string, binding, file="(eval)", line=1)
      cm = compile_eval string, binding.variables, file, line

      cm.scope = binding.static_scope
      cm.name = binding.variables.method.name

      # This has to be setup so __FILE__ works in eval.
      script = Rubinius::CompiledMethod::Script.new(cm, file, true)
      script.eval_source = string

      cm.scope.script = script

      be = Rubinius::BlockEnvironment.new
      be.under_context binding.variables, cm

      # Pass the BlockEnvironment this binding was created from
      # down into the new BlockEnvironment we just created.
      # This indicates the "declaration trace" to the stack trace
      # mechanisms, which can be different from the "call trace"
      # in the case of, say: eval("caller", a_proc_instance)
      if binding.from_proc?
        be.proc_environment = binding.proc_environment
      end

      be.from_eval!
     
      return be
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
