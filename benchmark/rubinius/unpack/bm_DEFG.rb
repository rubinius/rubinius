# -*- encoding: ascii-8bit -*-
require 'benchmark'

total = (ENV['TOTAL'] || 50_000).to_i

# Single-precision encoding
S = "\237+\003 _\240\002B\333\251\3155;A\217@n?\316P\036\210\303@?\337;\027\320\022cA\000=\221`\344X\341C\000=\221`\344X\341CR\236\363K\251\227\370?\000=\221`\344X\341Cn\335f\177\366G\217@\000\310Ngm\301\253C\000\001@\345\2340\242B\350\352\004 _\240\002B\324\032\004 _\240\002BZ\207\177\022PW%@\366\000\000\242\224\032mB7\311\000\350vH7B\325\320\251\346\245\320\373?\000\000\000\242\224\032mB\342\222\231\353\260\021\374?\000\240\330\205W4vC\000\0004&\365k\fC\000=\221`\344X\341C\000\0004&\365k\fC\000\000\220\036\304\274\326B\325\370\006 _\240\002Bf\001@\345\2340\242B-2\000 _\240\002B\000\200\3407y\303AC\361\360\233\034\341\306%@&\307y\000e\315\315A\240\365-\331\016j\370@\000=\221`\344X\341C\035\270\004 _\240\002Bd\e\000\350vH7B3\372\221\017\320\022cA_$\244\276W$Y@@\000\220\036\304\274\326B\3645\353\002\204\327\227A\000\200\3407y\303AC%A\323\372Ta\363?\000=\221`\344X\341C\000\200\3407y\303AC\020tt\001\320\022cApR\r\3733x%@\000\0004&\365k\fC\000\240\330\205W4vC\004V\a _\240\002B\006;\017\005\320\022cA\024\005\n\000e\315\315A7\2616\021\320\022cA"

# Double-precision encoding
D = "\237+\003 _\240\002B\333\251\3155;A\217@n?\316P\036\210\303@?\337;\027\320\022cA\000=\221`\344X\341C\000=\221`\344X\341CR\236\363K\251\227\370?\000=\221`\344X\341Cn\335f\177\366G\217@\000\310Ngm\301\253C\000\001@\345\2340\242B\350\352\004 _\240\002B\324\032\004 _\240\002BZ\207\177\022PW%@\366\000\000\242\224\032mB7\311\000\350vH7B\325\320\251\346\245\320\373?\000\000\000\242\224\032mB\342\222\231\353\260\021\374?\000\240\330\205W4vC\000\0004&\365k\fC\000=\221`\344X\341C\000\0004&\365k\fC\000\000\220\036\304\274\326B\325\370\006 _\240\002Bf\001@\345\2340\242B-2\000 _\240\002B\000\200\3407y\303AC\361\360\233\034\341\306%@&\307y\000e\315\315A\240\365-\331\016j\370@\000=\221`\344X\341C\035\270\004 _\240\002Bd\e\000\350vH7B3\372\221\017\320\022cA_$\244\276W$Y@@\000\220\036\304\274\326B\3645\353\002\204\327\227A\000\200\3407y\303AC%A\323\372Ta\363?\000=\221`\344X\341C\000\200\3407y\303AC\020tt\001\320\022cApR\r\3733x%@\000\0004&\365k\fC\000\240\330\205W4vC\004V\a _\240\002B\006;\017\005\320\022cA\024\005\n\000e\315\315A7\2616\021\320\022cA"


Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { 1 }
  end

  # Single-precision
  x.report("[S].unpack('F')") do
    total.times { S.unpack("F") }
  end

  x.report("[S].unpack('f')") do
    total.times { S.unpack("f") }
  end

  x.report("[S].unpack('e')") do
    total.times { S.unpack("e") }
  end

  x.report("[S].unpack('g')") do
    total.times { S.unpack("g") }
  end

  x.report("[S].unpack('F*')") do
    total.times { S.unpack("F*") }
  end

  x.report("[S].unpack('f*')") do
    total.times { S.unpack("f*") }
  end

  x.report("[S].unpack('e*')") do
    total.times { S.unpack("e*") }
  end

  x.report("[S].unpack('g*')") do
    total.times { S.unpack("g*") }
  end

  # Double-precision
  x.report("[D].unpack('D')") do
    total.times { D.unpack("D") }
  end

  x.report("[D].unpack('d')") do
    total.times { D.unpack("d") }
  end

  x.report("[D].unpack('E')") do
    total.times { D.unpack("E") }
  end

  x.report("[D].unpack('G')") do
    total.times { D.unpack("G") }
  end

  x.report("[D].unpack('D*')") do
    total.times { D.unpack("D*") }
  end

  x.report("[D].unpack('d*')") do
    total.times { D.unpack("d*") }
  end

  x.report("[D].unpack('E*')") do
    total.times { D.unpack("E*") }
  end

  x.report("[D].unpack('G*')") do
    total.times { D.unpack("G*") }
  end
end
