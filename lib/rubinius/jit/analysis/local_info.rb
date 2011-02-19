module Rubinius::JITCompiler
  class Analysis::LocalInfo < Analysis::Pass
    class Use
      def initialize(intro, use)
        @intro = intro
        @use = use
      end

      def dominates?(use)
        @use.dominates?(use.use)
      end

      attr_reader :intro, :use
    end

    class Local
      def initialize(idx)
        @idx = idx
        @writes = []
        @reads = []
      end

      attr_reader :writes, :reads

      def add_write(intro, use)
        @writes << Use.new(intro, use)
      end

      def add_read(intro, use)
        @reads << Use.new(intro, use)
      end

      def find_init
        sorted = @writes.sort do |a,b|
          if a.equal?(b)
            0
          elsif a.dominates?(b)
            1
          else
            -1
          end
        end

        init = sorted.first

        @reads.each do |r|
          return nil if r.dominates?(init)
        end

        return init
      end
    end

    def initialize
      @locals = Hash.new { |h,k| h[k] = Local.new(k) }
    end

    attr_reader :locals

    def [](blk)
      if @locals.key?(blk)
        @locals[blk]
      end
    end

    def run_on_block(pc, block)
      stores = {}

      block.each do |i|
        if i.kind_of? Read
          i.reads.each do |r|
            s = stores[r]
            if s.kind_of? GetLocal
              @locals[s.local].add_read(s, i)
            end
          end
        end

        case i
        when SetLocal
          pos = stores[i.source]
          @locals[i.local].add_write(pos, i)
        when Store
          stores[i.destination] = i
        end
      end
    end

    def after_run(pc)
      pc.local_info = self
    end

    def show
      @locals.each do |idx,info|
        puts "local #{idx}:"
        puts "  reads:"
        info.reads.each do |u|
          puts "    intro: #{u.intro.asm}"
          puts "      use: #{u.use.asm}"
        end
        puts "  writes:"
        info.writes.each do |u|
          puts "    intro: #{u.intro.asm}"
          puts "      use: #{u.use.asm}"
        end
      end
    end
  end
end
