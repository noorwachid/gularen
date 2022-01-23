" Vim syntax file
" Language:    Gularen
" Maintainer:  Noor Wachid
" Filenames:   *.gr
" Last Change: 2021
"

if exists('b:current_syntax')
  finish
endif

let b:current_syntax = 'gularen'

sy match gularenComment    "\^.*"
sy match gularenTitleArrow "^\ *>\{1,3\}"
sy match gularenTitleArrow "^\ *>>--\?>"
sy match gularenTitleArrow "^\ *--\?>"
sy match gularenBullet     "^\ *\[+\]\ "
sy match gularenBullet     "^\ *\[-\]\ "
sy match gularenBullet     "^\ *\[\ \]\ "
sy match gularenBullet     "^\ *-\ "
sy match gularenBullet     "^\ *\d\+\.\ "
sy match gularenBullet     "^\ *\.\.\ "
sy match gularenLine       "^\ *---\+"
sy match gularenBox        "^\ *\::"
sy match gularenRevTail    "<"
sy match gularenCB         "{\(:\|!\|?\)\?"
sy match gularenCB         "}"

sy region gularenFBold      start="\*" end="\*"
sy region gularenFItalic    start="_" end="_"
sy region gularenFMonospace start="`" end="`"
sy region gularenInlineCode start="``" end="``"

hi gularenFBold   gui=bold cterm=bold
hi gularenFItalic gui=italic cterm=italic

hi def url gularenComment    Comment
hi def url gularenTitleArrow Title
hi def url gularenBullet     Number
hi def url gularenLine       Function
hi def url gularenBox        Function
hi def url gularenRevTail    Function
hi def url gularenFMonospace Number
hi def url gularenInlineCode String
hi def url gularenCB         Keyword
