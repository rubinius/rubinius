def test_case
{"RawParseTree"=>
  [:array,
   [:back_ref, :&],
   [:back_ref, :`],
   [:back_ref, :"'"],
   [:back_ref, :+]],
 "Ruby"=>"[$&, $`, $', $+]",
 "RubyParser"=>
  s(:array,
   s(:back_ref, :&),
   s(:back_ref, :`),
   s(:back_ref, :"'"),
   s(:back_ref, :+))}
end
