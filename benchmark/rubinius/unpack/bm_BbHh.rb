# -*- encoding: ascii-8bit -*-
require 'benchmark'

total = (ENV['TOTAL'] || 500_000).to_i

S = "\207jN\353L\335\257\214\272\313\315\257\300\245\360\032\246\361\325\257\276\252\353\240\357\240"
M = "@\202\325-\001\e\252\361\312\330l\257c\310?:Q\020n\334\377\"\036\024\303j\335\032\b\354qohoc\335\r5\372\211\314\333\030\253\n\233\3744\376\315\240"
L = "H\272!\361\237A\336+0xG9J\356I\203'\321\365U\314) \273\217\254\334\a#\302m\263\032\334\333\226\003\\\022\260\365+\037\273\004\230\317\006\275\304F\317\377\276\366\375\325L\320.L\315\351\255\325\331\271\254.\016\323\227\364=\277 *\317\360\217}\302\230\267\354\333=N\363\r\377\331Np:,\r\332\341\276\306\214\017s\276\035\374\0053>\262\\\336\277\345\275\317\246\302\315\021\314\313\331\2256\302@"

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { 1 }
  end

  x.report("S.unpack('B')") do
    total.times { S.unpack('B') }
  end

  x.report("S.unpack('b')") do
    total.times { S.unpack('b') }
  end

  x.report("S.unpack('H')") do
    total.times { S.unpack('H') }
  end

  x.report("S.unpack('h')") do
    total.times { S.unpack('h') }
  end

  x.report("M.unpack('B20')") do
    total.times { M.unpack('B20') }
  end

  x.report("M.unpack('b20')") do
    total.times { M.unpack('b20') }
  end

  x.report("M.unpack('H5')") do
    total.times { M.unpack('H5') }
  end

  x.report("M.unpack('h5')") do
    total.times { M.unpack('h5') }
  end

  x.report("L.unpack('B*')") do
    total.times { L.unpack('B*') }
  end

  x.report("L.unpack('b*')") do
    total.times { L.unpack('b*') }
  end

  x.report("L.unpack('H*')") do
    total.times { L.unpack('H*') }
  end

  x.report("L.unpack('h*')") do
    total.times { L.unpack('h*') }
  end
end
