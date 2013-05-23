# -*- encoding: us-ascii -*-

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

    if RBC_DB = Rubinius::Config['rbc.db']
      def self.compiled_name(file)
        full = "#{File.expand_path(file)}#{Rubinius::RUBY_LIB_VERSION}"
        hash = Rubinius.invoke_primitive :sha1_hash, full
        dir = hash[0,2]

        "#{RBC_DB}/#{dir}/#{hash}"
      end
    else
      def self.compiled_cache_writable?(db, dir)
        File.owned?(db) or File.owned?(dir)
      end

      def self.compiled_name(file)
        name = File.expand_path file

        # If the file we are compiling is in a subdirectory of the current
        # working directory when Rubinius is started, try to cache the
        # compiled code in <cwd>/.rbx if:
        #
        #   1. <cwd>/.rbx exists and is owned by the process user
        #       OR
        #   2. if <cwd> is owned by the process user
        #
        # Otherwise, try to cache the compiled code in ~/.rbx if:
        #
        #   1. ~/.rbx exists and is owned by the process user
        #       OR
        #   2. ~/ is owned by the process user

        unless @db
          dir = Rubinius::OS_STARTUP_DIR
          db = "#{dir}/.rbx"
          unless name.prefix?(dir) and compiled_cache_writable?(db, dir)
            # Yes, this retarded shit is necessary because people actually
            # run under fucked environments with no HOME set.
            return unless ENV["HOME"]

            dir = File.expand_path "~/"
            db = "#{dir}/.rbx"
            return unless compiled_cache_writable?(db, dir)
          end
          @db = db
        end

        full = "#{name}#{Rubinius::RUBY_LIB_VERSION}"
        hash = Rubinius.invoke_primitive :sha1_hash, full

        "#{@db}/#{hash[0, 2]}/#{hash}"
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
      writer.version = Rubinius::RUBY_LIB_VERSION
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
      compiler = new :file, :compiled_code

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
      compiler = new :string, :compiled_code

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
      end

      attr_reader :current, :misses

      def clear!
        Rubinius.synchronize(self) do
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
        Rubinius.synchronize(self) do
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
        Rubinius.synchronize(self) do
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
        if code = ec.retrieve([string, layout, line])
          return code
        end
      end

      compiler = new :eval, :compiled_code

      parser = compiler.parser
      parser.root AST::EvalExpression
      parser.default_transforms
      parser.input string, file, line

      compiler.generator.variable_scope = variable_scope

      code = compiler.run

      code.add_metadata :for_eval, true

      if ec and parser.should_cache?
        ec.set([string.dup, layout, line], code)
      end

      return code
    end

    def self.construct_block(string, binding, file="(eval)", line=1)
      code = compile_eval string, binding.variables, file, line

      code.scope = binding.constant_scope
      code.name = binding.variables.method.name

      # This has to be setup so __FILE__ works in eval.
      script = Rubinius::CompiledCode::Script.new(code, file, true)
      script.eval_source = string

      code.scope.script = script

      be = Rubinius::BlockEnvironment.new
      be.under_context binding.variables, code

      # Pass the BlockEnvironment this binding was created from
      # down into the new BlockEnvironment we just created.
      # This indicates the "declaration trace" to the stack trace
      # mechanisms, which can be different from the "call trace"
      # in the case of, say: eval("caller", a_proc_instance)
      if binding.from_proc?
        be.proc_environment = binding.proc_environment
      end

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

    def self.compile_test_bytecode_19(string, transforms)
      compiler = new :string, :bytecode

      parser = compiler.parser
      parser.root AST::Snippet
      parser.processor Rubinius::Melbourne19
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
