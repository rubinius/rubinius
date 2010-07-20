class Dir
  module Glob
    class Node
      def initialize(nxt, flags)
        @flags = flags
        @next = nxt
        @seperator = nil
      end

      attr_writer :seperator

      def seperator
        @seperator || "/"
      end

      def path_join(parent, ent)
        return ent unless parent
        if parent == "/"
          "/#{ent}"
        else
          "#{parent}#{seperator}#{ent}"
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

      def match?(str)
        File.fnmatch @glob, str, @flags
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
      parts = glob.split(%r!(/+)!)

      if glob[-1] == ?/
        parts.pop # remove /+

        last = DirectoriesOnly.new nil, flags, parts.pop
        if parts.empty?
          last = RecursiveDirectories.new last, flags
        end
      else
        last = EntryMatch.new nil, flags, parts.pop
      end

      until parts.empty?
        last.seperator = parts.pop
        dir = parts.pop

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

    def self.glob(pattern, flags)
      if pattern.include? "{"
        return brace_glob(pattern, flags)
      end

      if node = compile(pattern, flags)
        run node
      else
        []
      end
    end

    def self.brace_glob(pattern, flags, matches=[])
      escape = (flags & File::FNM_NOESCAPE) == 0

      rbrace = nil
      lbrace = nil

      # Do a quick search for a { to start the search better
      i = pattern.index("{")

      # If there was a { found, then search
      if i
        nest = 0
        data = pattern.data
        total = pattern.size

        while i < total
          char = data.get_byte(i)

          if char == ?{ and nest == 0
            lbrace = i
            nest += 1
          end

          if char == ?} and nest - 1 <= 0
            rbrace = i
            nest -= 1
            break
          end

          if char == ?\\ and escape
            i += 1
          end

          i += 1
        end
      end

      # There was a full {} expression detected, expand each part of it
      # recursively.
      if lbrace and rbrace
        pos = lbrace
        front = pattern[0...lbrace]
        back = pattern[(rbrace + 1)..-1]

        while pos < rbrace
          nest = 0
          pos += 1
          last = pos

          while pos < rbrace and not (pattern[pos] == ?, and nest == 0)
            nest += 1 if pattern[pos] == ?{
              nest -= 1 if pattern[pos] == ?}

              if pattern[pos] == ?\\ and escape
                pos += 1
                break if pos == rbrace
              end

              pos += 1
          end

          brace_pattern = "#{front}#{pattern[last...pos]}#{back}"

          brace_glob brace_pattern, flags, matches
        end

        # No braces found, match the pattern normally
      else
        # Don't use .glob here because this code can detect properly
        # if a { is a brace or a just a normal character, but .glob can't.
        # if .glob is used and there is a { as a normal character, it will
        # recurse forever.
        if node = compile(pattern, flags)
          matches.concat run(node)
        end
      end

      return matches
    end
  end
end
