require 'benchmark'

total = (ENV['TOTAL'] || 50_000).to_i

S = "o\243\304=\320\211\247\001##"
M = "\004\036\355\270\224\017\346v\236\017\213.\325\224\310{C\245?\206\273\t\233\265\204"
L = "L6\334^A\267Mw\361A\231\036\376T_\357\226AC\331\303\233\267|\032t\210\344I\023\177\222\006\203\324\227\243\333\r\317>0\300\336\342\334T\322@5"
X = "\324:\363b\n-02>b\347D\251#\300\217\206=\247Q\334\374\203\222\205J\332\261]I7\261\357\321H\252\272\252\222ns\023\036\227\000\346\300\376\001\236\263\221j\e\242\271 e\201\372\305u\025J.V\254\025\020\255O\244\353\326\322\256]\aq\342bu\322U\306@S\231\321\232\306\357O(\000\270!\331\322&\3517j\e\245WU\177\030L\006\210\366bl-laW\304)L\242\352\250\\\3321\2367\277\272\305\361\037\236XK\247q\354\303G\331S]r\352\254\333\261\223\270c\236G\242\220X\244\310\3768\203C\256\272\224\312\227/\233h_\232\2632\f\221\e\215(G1Y\225\3138\026\021D\223\205\277E'M\334D*\036C`\216\256\250c]1>\001W\3529\026\353\332O\226Z\233k\316\a\344\t\255-\205\232\343\321\254\016E\235m\365\370)\322\r\177\272?\341?\"\216+s\aRw;"

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { 1 }
  end

  x.report("S.unpack('I')") do
    total.times { S.unpack("I") }
  end

  x.report("S.unpack('I*')") do
    total.times { S.unpack("I*") }
  end

  x.report("M.unpack('I*')") do
    total.times { M.unpack("I*") }
  end

  x.report("L.unpack('S*')") do
    total.times { L.unpack("S*") }
  end

  x.report("L.unpack('I*')") do
    total.times { L.unpack("I*") }
  end

  x.report("L.unpack('L*')") do
    total.times { L.unpack("L*") }
  end

  x.report("L.unpack('N*')") do
    total.times { L.unpack("N*") }
  end

  x.report("L.unpack('V*')") do
    total.times { L.unpack("V*") }
  end

  x.report("L.unpack('Q*')") do
    total.times { L.unpack("Q*") }
  end

  x.report("X.unpack('I*')") do
    total.times { X.unpack("I*") }
  end
end
