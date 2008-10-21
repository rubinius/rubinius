def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :argl, [:lit, 1]],
   [:dstr, "x", [:evstr, [:lvar, :argl]], [:str, "y"]]],
 "Ruby"=>"argl = 1\n\"x\#{argl}y\"\n",
 "RubyParser"=>
  s(:block,
   s(:lasgn, :argl, s(:lit, 1)),
   s(:dstr, "x", s(:evstr, s(:lvar, :argl)), s(:str, "y")))}
end
