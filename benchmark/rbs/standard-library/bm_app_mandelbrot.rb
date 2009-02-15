require 'complex'

def mandelbrot?(z)
  i = 0
  while i<100
    i+=1
    z = z * z
    return false if z.abs > 2
  end
  true
end

Bench.run [1] do |n|
  ary = []

  (0..100).each do |dx|
    (0..100).each do |dy|
      x = dx / 50.0
      y = dy / 50.0
      c = Complex(x, y)
      ary << c if mandelbrot?(c)
    end
  end
end
