class Dir
  module Glob
    class Node
      def initialize(nxt)
        @next = nxt
      end

      def path_join(parent, ent)
        return ent unless parent
        if parent == "/"
          "/#{ent}"
        else
          "#{parent}/#{ent}"
        end
      end
    end

    class ConstantDirectory < Node
      def initialize(nxt, dir)
        super nxt
        @dir = dir
      end

      def call(env, path)
        full = path_join(path, @dir)

        if File.directory? full
          @next.call env, full
        end
      end
    end

    class RootDirectory < Node
      def call(env, path)
        @next.call env, "/"
      end
    end

    class RecursiveDirectories < Node
      def call(env, start)
        if start.nil? or File.exists? start
          @next.call env, start
        end

        stack = [start]

        until stack.empty?
          path = stack.pop
          dir = Dir.new(path ? path : ".")
          while ent = dir.read
            next if ent == "." || ent == ".."
            full = path_join(path, ent)

            if File.directory? full
              stack << full
              @next.call env, full
            end
          end
          dir.close
        end
      end
    end

    class EachDirectory < Node
      def call(env, path)
        dir = Dir.new(path)
        while ent = dir.read
          @next.call env, ent
        end
        dir.close
      end
    end

    class Match < Node
      def initialize(nxt, glob)
        super nxt
        @glob = glob
        # @pattern = to_regex glob
        # p @pattern
      end

      Escape = %w!+ | ^ $ ( ) [ ]!
      def to_regex(glob)
        str = glob.gsub ".", "\\."
        Escape.each do |char|
          str.gsub! char, "\\#{char}"
        end
        str.gsub! /[^\\]\*/, ".+"
        str.gsub! /[^\\]\?/, "."
        # handle {}

        Regexp.new "^#{str}$"
      end

      def match?(str)
        Platform::POSIX.fnmatch(@glob, str, 4) == 0
      end
    end

    class DirectoryMatch < Match
      def initialize(nxt, glob)
        super

        @glob.gsub! "**", "*"
      end

      def call(env, path)
        dir = Dir.new(path ? path : ".")
        while ent = dir.read
          if match? ent
            full = path_join(path, ent)

            if File.directory? full
              @next.call env, full
            else
              env.matches << full
            end
          end
        end
        dir.close
      end
    end

    class EntryMatch < Match
      def call(env, path)
        begin
          dir = Dir.new(path ? path : ".")
        rescue Errno::EACCES
          return
        end

        while ent = dir.read
          if match? ent
            env.matches << path_join(path, ent)
          end
        end
        dir.close
      end
    end

    class DirectoriesOnly < Match
      def call(env, path)
        dir = Dir.new(path ? path : ".")
        while ent = dir.read
          full = path_join(path, ent)

          if File.directory? full and match? ent
            env.matches << "#{full}/"
          end
        end
        dir.close
      end
    end

    class Environment
      attr_reader :matches

      def initialize
        @matches = []
      end
    end

    def self.compile(glob)
      parts = glob.split("/")

      file = parts.pop

      if glob[-1] == ?/
        last = DirectoriesOnly.new nil, file
        if parts.empty?
          last = RecursiveDirectories.new last
        end
      else
        last = EntryMatch.new nil, file
      end

      parts.reverse_each do |dir|
        if dir == "**"
          last = RecursiveDirectories.new last
        elsif /[a-zA-Z0-9]+/.match(dir)
          last = ConstantDirectory.new last, dir
        elsif !dir.empty?
          last = DirectoryMatch.new last, dir
        end
      end

      if glob[0] == ?/
        last = RootDirectory.new last
      end

      last
    end

    def self.run(node)
      env = Environment.new
      node.call env, nil
      env.matches
    end
  end
end
