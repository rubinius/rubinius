##
# Ar reads ar(5) formatted files.
class Ar

  class Error < RuntimeError; end

  def initialize(path)
    @path = path
  end

  ##
  # Removes +files+ from the archive.
  def delete(*files)
    require 'fileutils'
    require 'tempfile'

    Tempfile.open "#{File.basename @path}.new" do |io|
      io.write "!<arch>\n"

      each do |name, mtime, uid, gid, mode, data|
        next if files.include? name

        write_file io, name, mtime, uid, gid, mode, data
      end

      FileUtils.mv io.path, @path
    end

    self
  end

  ##
  # Yields each archive item's metadata and data.
  # 
  # @see Array#each
  def each
    open @path, 'rb' do |io|
      raise Error, "#{@path} is not an archive file" if io.gets != "!<arch>\n"

      until io.eof? do
        name  = io.read 16
        mtime = io.read(12).to_i
        mtime = Time.at(mtime).utc
        uid   = io.read( 6).to_i
        gid   = io.read( 6).to_i
        mode  = io.read( 8).to_i(8)
        size  = io.read(10).to_i
                io.read  2 # trailer

        name = if name =~ /^#1\/(\d+)/ then
                 name_length = $1.to_i
                 size -= name_length
                 io.read(name_length).delete "\000"
               else
                 name.rstrip
               end

        data = io.read size
        io.read 1 if size % 2 == 1

        yield name, mtime, uid, gid, mode, data
      end
    end
  end

  ##
  # Exctracts metadata and data for +file+.  Returns an Array containing the
  # name, last modification time, uid, gid, mode and archive item data.
  def extract(file)
    find do |name,|
      file == name
    end
  end

  ##
  # Lists the files in the archive in order.
  # 
  # @return [Array<String>] the names of the files in order
  def list
    map {|name,| name }
  end

  ##
  # Adds or replaces the file +name+ in the archive.  If the file already
  # exists, it is moved to the end of the archive.
  # 
  # @return [Ar] chainable
  def replace(name, mtime, uid, gid, mode, data)
    if File.exist? @path then
      delete name if list.include? name
    else
      open @path, 'ab' do |io| io.write "!<arch>\n" end
    end

    open @path, 'ab' do |io|
      write_file io, name, mtime.to_i, uid, gid, mode, data
    end

    self
  end

  ##
  # Writes the archive to +io+.
  # 
  # @return [Ar] chainable
  def write(io)
    io.write "!<arch>\n"

    each do |name, mtime, uid, gid, mode, data|
      write_file io, name, mtime, uid, gid, mode, data
    end

    self
  end

  def write_file(io, name, mtime, uid, gid, mode, data) # :nodoc:
    unless name.length > 16 then
      padding = nil
      io.write name.ljust(16)
    else
      padding = 4 - name.length % 4
      padding = 0 if padding > 3
      io.write "#1/#{name.length + padding}".ljust(16)
    end

    io.write mtime      .to_i.to_s.ljust(12)
    io.write uid        .to_s     .ljust( 6)
    io.write gid        .to_s     .ljust( 6)
    io.write mode       .to_s(8)  .ljust( 8)
    size = data.length + (padding ? name.length + padding : 0)
    io.write size       .to_s     .ljust(10)
    io.write "`\n"

    if name.length > 16 then
      name = name.ljust name.length + padding, "\000"
      io.write name
    end

    io.write data
    io.write "\n" if data.length % 2 == 1

    self
  end
end
