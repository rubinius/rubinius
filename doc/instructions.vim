if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn include @CPPTop syntax/cpp.vim

syn region instructionBody matchgroup=Delimiter start="instruction" end="end" contains=@CPPTop
syn match instructionComment "#.*"

" Set up default highlighting. Almost all of this is done in the included
" syntax files.
"
if version >= 508 || !exists("did_instruction_syn_inits")
  if version < 508
    let did_instruction_syn_inits = 1
    com -nargs=+ HiLink hi link <args>
  else
    com -nargs=+ HiLink hi def link <args>
  endif

  HiLink instructionComment Comment
  delcommand HiLink
endif

se indentexpr=

let b:current_syntax = "instructions"
