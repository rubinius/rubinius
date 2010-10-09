require 'benchmark'

N = (ENV['N'] || 500_000).to_i

A = ["h|4c@jcb av8\nPvun", "eg nm-yg vmnb ", "lh", "g\ndqldeg", " lz
  kdgy7hlK", "y lz  kdgy7hlKlR n", "gy7hlKlR nzqxg\ndqlde", " lz  kdgy7hlKlR
  nz", "qxg\ndqldeg nm-yg ", "nqsp uxq\nhnokjirs\nb c", "", "qldeg nm-",
  "okjirs\nb c6rlh|", "nszwijhy lz  kdgy7h", "v8\nPvunszwijhy lz  k", "g vmnb
  mk gdrn  ", "m-yg vmnb m", " nzqxg\ndqldeg nm-yg ", "hnokjir",
  "zqxg\ndqlde", "lz  kdgy7hlKlR nz", "eg nm-yg vmnb mk gdrn  ", "ijhy lz
  kdgy7hlKlR", " nzqxg\ndqldeg nm-yg vm", "xg\ndqldeg nm-yg vmnb mk ", " n", "
  ", "qsp ux", "jcb av8\nPvunszwijhy l", "|4c@jcb av8\nPv", " c6rlh|4c@jcb",
  "hn", "xg\ndq", "  kdgy7h", "4c@jcb av8\nPvun", "ldeg nm-yg vmnb mk",
  "qxg\ndqldeg nm-y", "mk gdrn", " nm-yg vmnb mk gdr", " ", "uxq\nhnok",
  "dnqsp uxq\nhnokjirs\nb ", "uxq\nhnokj", "lz  kdgy7hlKlR nzqxg\n", "szwijhy
  l", "deg nm-yg", "p uxq\nhnokjirs\nb c6rlh|4", "qldeg nm-yg vmnb m", "hlKlR
  nzqxg\ndqldeg ", "unszwijhy lz  kdgy7hlK"]
B = ["Oeg nm-yg vmn", "\ndNlO", "gy7", "\run", "okj,rk\nb c6rlh", " vmn\253 m-
  g\245rZ  ", "R \245\262qxg\nd", "\211 kdgy", "7\355lKlR \245\262qxg\ndNlOeg
  n", "xg\ndNlOeg nm", " m- g\245r", "xq\nhnokj,rk\nb c6rlh|4}@",
  "\e\nP\runszwijhy\223lz", "lh|4}@j", "\242nX\023\254 uxq\nhnokj,rk\nb", "g
  vmn\253 m", "7", " \245\262qxg\ndNlOeg ", "\e\nP\ru", " av", "y7\355lKlR
  \245\262qxg\ndNlOeg nm-", "\runszwijhy\223lz\211 k", "y7\355", "gy7\355lKlR
  \245\262qxg\ndNl", "m-yg vmn\253 m- ", "\242nX\023\254 uxq\nhnokj,rk\nb c6",
  "\n", "\ndNl", "lKlR \245\262qxg\n", "y\223lz\211 kdgy7\355lKlR \245\262q",
  "\nP\runszwijhy\223l", "b c6rlh|4}@jcb av\e\nP\run", "", "j,rk\nb c6rl",
  "zwijh", "xq\nhnokj,rk\nb c6rlh|4}@", "r\242nX\023\254 uxq\nh", "\223lz\211
  kdgy7\355l", "P\runszwijhy\223lz\211 kdgy7\355l", "\245\262qxg\ndNlOeg nm-yg
  vmn\253", "hy\223lz\211 kdgy", "lKlR \245\262qxg\ndNlOeg nm-",
  "wijhy\223lz\211 kdgy7\355lK", "nm-yg", "@jcb av\e\nP\runszwij", " m- g",
  "X\023\254 uxq\nh", "nszwijhy\223lz\211 kdgy", "h|4}@jcb av", "-yg vm"]

def casecmp_loop(ss)
  i = 0
  total = ss.size
  while i < total
    ss[i]
    ss[i+1]
    i += 2
  end
end

def casecmp(ss)
  i = 0
  total = ss.size
  while i < total
    ss[i].casecmp ss[i+1]
    i += 2
  end
end

Benchmark.bmbm do |x|
  x.report("loop") { N.times { casecmp_loop A } }
  x.report("casecmp A") { N.times { casecmp A } }
  x.report("casecmp B") { N.times { casecmp B } }
end
