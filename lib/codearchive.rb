class CodeArchive
  def initialize(path)
    @path = path
    reload()
  end
  
  def reload
    if File.exists?(@path)
      @data = Archive.list_files(@path)
      @times = {}
      @data.each do |info|
        @times[info[0]] = info[1]
      end
    else
      @data = []
      @times = {}
    end
  end
  
  attr_accessor :data
  attr_accessor :times
  
  def files
    out = []
    @data.each { |t| out << t.at(0) }
    return out
  end
  
  def last_modified(file)
    if time = @times[file]
      return time
    elsif file.suffix?(".rb")
      return @times["#{file}c"]
    else
      return nil
    end
  end
  
  def out_of_date?(file)
    cur = last_modified(file)
    return true unless cur
    
    stat = File.stat(file)
    stat.mtime > cur
  end
  
  def find_out_of_date(dir)
    ood = []
    Dir["#{dir}/*.rb"].each do |file|
      ood << file if out_of_date?(file)
    end
    return ood
  end
  
  def add_file(name, method)
    # Save blah.rb as blah.rbc
    if name.suffix?(".rb")
      name = "#{name}c"
    end
    
    Archive.add_object @path, name, method
    reload()
    return true
  end
    
  def refresh_file(file)
    return false unless out_of_date?(file)
    
    meth = Compiler::Utils.compile_file(file)
    add_file(file, meth)
  end
  
  def update_from_directory(dir)
    added = find_out_of_date(dir)
    added.each do |file|
      yield file if block_given?
      meth = Compiler::Utils.compile_file(file)
      add_file(file, meth)
    end
    
    return added
  end
end
