class Dir
  module Glob
    class Node
      def initialize(nxt, flags)
        @flags = flags
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
      def initialize(nxt, flags, dir)
        super nxt, flags
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
        allow_dots = ((@flags & File::FNM_DOTMATCH) != 0)

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

            if File.directory? full and ent[0] != ?.
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
      def initialize(nxt, flags, glob)
        super nxt, flags
        @glob = glob
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
        File.fnmatch @glob, str, @flags
        # FFI::Platform::POSIX.fnmatch(@glob, str, 4) == 0
      end
    end

    class DirectoryMatch < Match
      def initialize(nxt, flags, glob)
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
        allow_dots = ((@flags & File::FNM_DOTMATCH) != 0)
        all_dots = (@glob[0] == ?.)

        dir = Dir.new(path ? path : ".")
        while ent = dir.read
          unless all_dots
            if ent[0] == ?.
              next if ent.size == 1 or ent[1] == ?.
              next unless allow_dots
            end
          end

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

    def self.compile(glob, flags=0)
      parts = glob.split("/")

      file = parts.pop

      if glob[-1] == ?/
        last = DirectoriesOnly.new nil, flags, file
        if parts.empty?
          last = RecursiveDirectories.new last, flags
        end
      else
        last = EntryMatch.new nil, flags, file
      end

      parts.reverse_each do |dir|
        if dir == "**"
          last = RecursiveDirectories.new last, flags
        elsif /[a-zA-Z0-9.]+/.match(dir)
          last = ConstantDirectory.new last, flags, dir
        elsif !dir.empty?
          last = DirectoryMatch.new last, flags, dir
        end
      end

      if glob[0] == ?/
        last = RootDirectory.new last, flags
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
