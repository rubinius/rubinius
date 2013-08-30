# -*- encoding: us-ascii -*-

class File

  def self.expand_path(path, dir=nil)
    path = Rubinius::Type.coerce_to_path(path)
    enc = path.encoding
    first = path[0]
    if first == ?~
      case path[1]
      when ?/
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        path = ENV["HOME"] + path.byteslice(1, path.bytesize - 1)
      when nil
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        if home.empty?
          raise ArgumentError, "HOME environment variable is empty expanding '~'"
        end

        return home.dup
      else
        unless length = path.find_string("/", 1)
          length = path.bytesize
        end

        name = path.byteslice 1, length - 1
        unless dir = Rubinius.get_user_home(name)
          raise ArgumentError, "user #{name} does not exist"
        end

        path = dir + path.byteslice(length, path.bytesize - length)
      end
    elsif first != ?/
      if dir
        dir = expand_path dir
      else
        dir = PrivateDir.pwd
      end

      path = "#{dir}/#{path}"
    end

    items = []
    start = 0
    size = path.bytesize

    while index = path.find_string("/", start) or (start < size and index = size)
      length = index - start

      if length > 0
        item = path.byteslice start, length

        if item == ".."
          items.pop
        elsif item != "."
          items << item
        end
      end

      start = index + 1
    end

    if items.empty?
      str = "/"
    else
      str = ""
      items.each { |x| str.append "/#{x}" }
    end

    return Rubinius::Type.external_string(str).encode(enc)
  end

end
