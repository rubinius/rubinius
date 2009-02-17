#  The Computer Language Benchmarks Game
#  http://shootout.alioth.debian.org/
#
#  contributed by Karl von Laudermann
#  modified by Jeremy Echols
#  modified by Detlef Reichl

size = 1000
puts "P4\n#{size} #{size}"

ITER = 50           # Mandelbrot iterations
LIMIT_SQUARED = 4.0 # Presquared limit

Bench.run [1] do |n|
  byte_acc = 0
  bit_num = 0
  count_size = size - 1 # Precomputed size for easy for..in looping
  # For..in loops are faster than .upto, .downto, .times, etc.
  # That's not true, but left it here
  for y in 0..count_size
    for x in 0..count_size
      zr = 0.0
      zi = 0.0
      cr = (2.0*x/size)-1.5
      ci = (2.0*y/size)-1.0
      escape = false

      zrzr = zr*zr
      zizi = zi*zi
      ITER.times do
        tr = zrzr - zizi + cr
        ti = 2.0*zr*zi + ci
        zr = tr
        zi = ti
        # preserve recalculation
        zrzr = zr*zr
        zizi = zi*zi
        if zrzr+zizi > LIMIT_SQUARED
          escape = true
          break
        end
      end

      byte_acc = (byte_acc << 1) | (escape ? 0b0 : 0b1)
      bit_num += 1

      # Code is very similar for these cases, but using separate blocks
      # ensures we skip the shifting when it's unnecessary, which is most cases.
      if (bit_num == 8)
        print byte_acc.chr
        byte_acc = 0
        bit_num = 0
      elsif (x == count_size)
        byte_acc <<= (8 - bit_num)
        print byte_acc.chr
        byte_acc = 0
        bit_num = 0
      end
    end
  end
end
