require 'rubygems'
require 'png'

class MemoryRenderer
  
  def initialize(width, height)
    @width = width
    @height = height
    @max = @width * @height
  end
  
  def coords_for(address)
    index = address / 100
    
    if index > @max
      raise "Ran out of plot space at index %d for 0x%x" % [index, address]
    end
    
    x = index % @width
    y = index / @width
    
    return [x,y]
  end
  
  def consume(fd)
    @objects = []
    while line = fd.gets
      a, s, k = line.split(" ")
      @objects << [a.to_i, s.to_i, k.to_sym]
    end
  end
  
  def nc
    out = [@x, @y]
    @x += 1
    if @x >= @width
      @y += 1
      p @y
      if @y >= @height
        raise "flowed off the bottom"
      end
      @x = 0
    end
    return out  
  end
  
  def draw(file)
    canvas = PNG::Canvas.new @width, @height, PNG::Color::Black
    @x = 0
    @y = 0
    @objects.each do |object, size, kind|      
      
      color = case kind
              when :free      then PNG::Color::Gray
              when :cmethod   then PNG::Color::Red
              when :bytearray then PNG::Color::Yellow
              when :string    then PNG::Color::Purple
              when :tuple     then PNG::Color::Orange
              when :context   then PNG::Color::White
              else                 PNG::Color::Green
      end
      
      canvas[*nc] = PNG::Color::Red
      
      tms = size / 4
      
      tms.times do
        canvas[*nc] = color
      end
    end
  ensure
    png = PNG.new canvas
    png.save file
  end
end

png = ARGV.pop
mr = MemoryRenderer.new(800, 4000)
r, w = IO.pipe
pid = fork {
  r.close
  ENV["MEMORYFD"] = w.to_i.to_s
  exec "./shotgun/rubinius ./lib/kernel.rbc #{ARGV.shift}"
}
w.close
mr.consume(r)
Process.wait(pid)

mr.draw(png)