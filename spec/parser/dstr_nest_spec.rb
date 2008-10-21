def test_case
{"RawParseTree"=>
  [:dstr, "before [", [:evstr, [:vcall, :nest]], [:str, "] after"]],
 "Ruby"=>"%Q[before [\#{nest}] after]",
 "RubyParser"=>
  s(:dstr,
   "before [",
   s(:evstr, s(:call, nil, :nest, s(:arglist))),
   s(:str, "] after")),
 "Ruby2Ruby"=>"\"before [\#{nest}] after\""}
end
