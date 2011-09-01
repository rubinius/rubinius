require 'benchmark'
require 'benchmark/ips'

rules = [
  [/(z)ombies$/i, "\\1ombies"],
  [/(z)ombie$/i, "\\1ombies"],
  [/k(?i)ine$/, "kine"],
  [/K(?i)ine$/, "Kine"],
  [/c(?i)ow$/, "kine"],
  [/C(?i)ow$/, "Kine"],
  [/(m)oves$/i, "\\1oves"],
  [/(m)ove$/i, "\\1oves"],
  [/(s)exes$/i, "\\1exes"],
  [/(s)ex$/i, "\\1exes"],
  [/(c)hildren$/i, "\\1hildren"],
  [/(c)hild$/i, "\\1hildren"],
  [/(m)en$/i, "\\1en"],
  [/(m)an$/i, "\\1en"],
  [/(p)eople$/i, "\\1eople"],
  [/(p)erson$/i, "\\1eople"],
  [/(quiz)$/i, "\\1zes"],
  [/^(oxen)$/i, "\\1"],
  [/^(ox)$/i, "\\1en"],
  [/([m|l],)ice$/i, "\\1ice"],
  [/([m|l],)ouse$/i, "\\1ice"],
  [/(matr|vert|ind)(?:ix|ex)$/i, "\\1ices"],
  [/(x|ch|ss|sh)$/i, "\\1es"],
  [/([^aeiouy],|qu)y$/i, "\\1ies"],
  [/(hive)$/i, "\\1s"],
  [/(?:([^f],)fe|([lr],)f)$/i, "\\1\\2ves"],
  [/sis$/i, "ses"],
  [/([ti],)a$/i, "\\1a"],
  [/([ti],)um$/i, "\\1a"],
  [/(buffal|tomat)o$/i, "\\1oes"],
  [/(bu)s$/i, "\\1ses"],
  [/(alias|status)$/i, "\\1es"],
  [/(octop|vir)i$/i, "\\1i"],
  [/(octop|vir)us$/i, "\\1i"],
  [/(ax|test)is$/i, "\\1es"],
  [/s$/i, "s"],
  [/$/, "s"]
]


Benchmark.ips do |x|
  x.report "lots" do
    rules.each do |(rule, rep)|
      "javascript".gsub!(rule, rep)
    end
  end

  x.report "lots - ()" do
    rules.each do |rule, rep|
      "javascript".gsub!(rule, rep)
    end
  end

  str = "javascript"
  x.report "one" do
    str.gsub!(/^(oxen)$/i, "\\1")
  end

  x.report "one + newstr" do
    "javascript".gsub!(/^(oxen)$/i, "\\1")
  end

  if defined?(Rubinius)
    x.report "match_from" do
      /^(oxen)$/i.match_from "javascript", 0
    end
  end
end
